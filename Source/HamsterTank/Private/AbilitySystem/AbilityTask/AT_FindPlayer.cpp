// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AT_FindPlayer.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameClasses/TanksterGameplayTags.h"
#include "Kismet/GameplayStatics.h"

UAT_FindPlayer::UAT_FindPlayer()
{
	bTickingTask = false;
}

UAT_FindPlayer* UAT_FindPlayer::FindPlayer(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const float InTimerRate, const float InMaxRange,
                                           const float InFOVRadians, USceneComponent* InOriginComponent)
{
	UAT_FindPlayer* NewTask = NewAbilityTask<UAT_FindPlayer>(OwningAbility, TaskInstanceName);
	if(NewTask != nullptr)
	{
		NewTask->MaxRange = InMaxRange;
		NewTask->FOVRadians = InFOVRadians;
		NewTask->OriginComponent = InOriginComponent;
		NewTask->TimerRate = InTimerRate;
	}
	return NewTask;
}

void UAT_FindPlayer::Activate()
{
	Super::Activate();
	
	QueryParams.AddIgnoredActor(GetAvatarActor());
	
	check(GetWorld())
	GetWorld()->GetTimerManager().SetTimer(FindPlayerTimerHandle, this, &ThisClass::FindPlayer, TimerRate, true);
}

void UAT_FindPlayer::OnDestroy(bool bInOwnerFinished)
{

	PlayerFoundDelegate.Clear();
	PlayerLostDelegate.Clear();
	Super::OnDestroy(bInOwnerFinished);
}

TWeakObjectPtr<APawn> UAT_FindPlayer::GetPlayerPawn() const
{
	return UGameplayStatics::GetPlayerPawn(this, 0);
}

TWeakObjectPtr<UAbilitySystemComponent> UAT_FindPlayer::GetTargetASC(const TWeakObjectPtr<APawn> InTargetPawn)
{
	const IAbilitySystemInterface* IAB = Cast<IAbilitySystemInterface>(InTargetPawn);
	return IAB != nullptr && IAB->GetAbilitySystemComponent() != nullptr ? IAB->GetAbilitySystemComponent() : nullptr;
}

void UAT_FindPlayer::FindPlayer()
{
	const TWeakObjectPtr<APawn> TargetPawn = GetPlayerPawn();
	const TWeakObjectPtr<UAbilitySystemComponent> TargetASC = GetTargetASC(TargetPawn);

	if(!TryFindPlayer(TargetPawn, TargetASC))
	{
		PlayerLost();
		return;
	}
	else
	{
		PlayerFound(TargetPawn);
	}

	
	
}

bool UAT_FindPlayer::TryFindPlayer(TWeakObjectPtr<APawn> InTargetPawn, TWeakObjectPtr<UAbilitySystemComponent> InTargetASC) const
{
	return IsTargetInRange2D(InTargetPawn) && IsTargetAlive(InTargetASC) && IsTargetInFOV(InTargetPawn) && IsVisionToTargetUnblocked(InTargetPawn);
}

bool UAT_FindPlayer::IsTargetInRange2D(const TWeakObjectPtr<APawn> InTargetPawn) const
{
	if(InTargetPawn.IsValid() && OriginComponent.IsValid())
	{
		const FVector TargetLocation = InTargetPawn->GetActorLocation();
		const FVector OriginLocation = OriginComponent->GetComponentLocation();
		return FVector::Dist2D(TargetLocation, OriginLocation) <= MaxRange;
	}
	return false;
}

bool UAT_FindPlayer::IsTargetAlive(const TWeakObjectPtr<UAbilitySystemComponent> InTargetASC)
{
	return InTargetASC.IsValid() && !InTargetASC->HasMatchingGameplayTag(FTanksterGameplayTags::Get().Dead);
}

bool UAT_FindPlayer::IsTargetInFOV(TWeakObjectPtr<APawn> InTargetPawn) const
{
	if(InTargetPawn.IsValid() && OriginComponent.IsValid())
	{
		const FVector TargetLocation = InTargetPawn->GetActorLocation();
		const FVector OriginLocation = OriginComponent->GetComponentLocation();
		
		const FVector NormalizedTargetDirection = (TargetLocation - OriginLocation).GetSafeNormal2D();
		const float RadiansDistance = acos(OriginComponent->GetForwardVector().Dot(NormalizedTargetDirection));
		
		return RadiansDistance < FOVRadians;
	}
	return false;
}

bool UAT_FindPlayer::IsVisionToTargetUnblocked(TWeakObjectPtr<APawn> InTargetPawn) const
{
	const FVector TargetLocation = InTargetPawn->GetActorLocation();
	const FVector OriginLocation = OriginComponent->GetComponentLocation();
	
	FHitResult Result;

	GetWorld()->LineTraceSingleByChannel(Result, OriginLocation, TargetLocation, ECC_GameTraceChannel2, QueryParams);
	
	return (!Result.bBlockingHit || Result.GetActor() == InTargetPawn);
}

void UAT_FindPlayer::PlayerFound(TWeakObjectPtr<APawn> InFoundPlayer)
{
	if(!bPlayerFound || FoundPlayer != InFoundPlayer)
	{
		bPlayerFound = true;
		FoundPlayer = InFoundPlayer;
		PlayerFoundDelegate.Broadcast(FoundPlayer.Get());
	}
}

void UAT_FindPlayer::PlayerLost()
{
	if(bPlayerFound)
	{
		bPlayerFound = false;
		PlayerLostDelegate.Broadcast(FoundPlayer.Get());
		FoundPlayer = nullptr;
	}
}
