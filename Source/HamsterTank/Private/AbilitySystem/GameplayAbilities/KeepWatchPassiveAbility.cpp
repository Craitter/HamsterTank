// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbilities/KeepWatchPassiveAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTask/AT_FindPlayer.h"
#include "AbilitySystem/AbilityTask/AT_LookAtDynamicLocation.h"
#include "AbilitySystem/AbilityTask/AT_RotateIdleSin.h"
#include "AbilitySystem/GameplayAbilities/FireProjectileAbility.h"
#include "Actors/EnemyTower.h"
#include "GameClasses/TanksterGameplayTags.h"

void UKeepWatchPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	const TWeakObjectPtr<AEnemyTower> OwningTower = GetOwningTower();
	StartIdleRotation(OwningTower);
	StartKeepWatch(OwningTower);
	GetTowerType(OwningTower);
	ListenCooldownTagChanged();
	
	/**
	 *	1. AbilityTask (Ticks) rotates at given speed and range
	 *		Speed and Rotation is editable in instance			Done?
	 *	2. (Async?)Task (Checks if Player is found) forever?
	 *		Distance, Angle, Blocked?
	 *	3. Apply GE on Player which fires Ability to Predict
	 *			Wait Next Tick
	 *	4. Rotate to Predicted Position (Ability Task?)
	 *		Stops 1.
	 *	5. Fire Projectile (Modified)
	 *
	 *	6. If 2. returns false we stop 4. and wait for XY
	 *
	 *	7. After XY we start 1. again
	 *
	 */
	
	
}

void UKeepWatchPassiveAbility::OnPlayerFound(APawn* TargetPawn)
{
	NotifyTargetForPrediction(TargetPawn);
	StopIdleRotation();
	StartLookAtTargetLocation(TargetPawn);

	if(WaitForPlayerReturn.IsValid() && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitForPlayerReturn);
	}
	//PauseRotation
	//Apply GE to Player
	//Wait for Data Ready
	//Init Task Rotate to Location
	//Fire if reached
	
}

void UKeepWatchPassiveAbility::OnPlayerLost(APawn* TargetPawn)
{
	NotifyTargetStopPrediction(TargetPawn);
	PauseLookAtTargetLocation();
	check(GetWorld())
	const TWeakObjectPtr<AEnemyTower> Tower = GetOwningTower();
	if(Tower.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(WaitForPlayerReturn, this, &ThisClass::OnWaitForPlayerEnds, Tower->TimeBeforeRotatingBackToIdle);	
	}
	//Wait X Seconds
	//Remove GE
	//EndTask Rotate To Location
	//UnPause Rotation
}

void UKeepWatchPassiveAbility::OnTargetLocationReached(const float& Distance)
{
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = GetAbilitySystemComponentFromActorInfo();
	if(ASC.IsValid() && ASC->TryActivateAbilityByClass(UFireProjectileAbility::StaticClass()) && LookAtLocationTask != nullptr)
	{
		LookAtLocationTask->SetIsOnCooldown(true);
	}
}

void UKeepWatchPassiveAbility::OnFireProjectileCooldownTagChanged(FGameplayTag ChangedTag, int NewCount) const
{
	if(LookAtLocationTask == nullptr)
	{
		return;
	}
	LookAtLocationTask->SetIsOnCooldown(NewCount < 1);
}

void UKeepWatchPassiveAbility::StopIdleRotation() const
{
	if(RotateSinTask != nullptr)
	{
		RotateSinTask->EndTask();
	}
}

TWeakObjectPtr<AEnemyTower> UKeepWatchPassiveAbility::GetOwningTower() const
{
	return Cast<AEnemyTower>(GetAvatarActorFromActorInfo());
}


TWeakObjectPtr<UAbilitySystemComponent> UKeepWatchPassiveAbility::GetTargetAbilitySystemComponent(
	const TWeakObjectPtr<APawn> TargetPawn)
{
	const IAbilitySystemInterface* IAB = Cast<IAbilitySystemInterface>(TargetPawn);
	return IAB ? IAB->GetAbilitySystemComponent() : nullptr;
}

void UKeepWatchPassiveAbility::GetTowerType(const TWeakObjectPtr<AEnemyTower> Tower)
{
	if(Tower.IsValid())
	{
		TowerType = Tower->TowerType;
	}
	else
	{
		TowerType = ETowerType::Max;
	}
}

void UKeepWatchPassiveAbility::StartIdleRotation(const TWeakObjectPtr<AEnemyTower> Tower)
{
	if(Tower.IsValid())
	{
		const float RotationRange = Tower->YawRotationRange;
		const float AverageDegree = Tower->AverageDegreePerSecond;
		const TWeakObjectPtr<USceneComponent> RotationComp = Tower->GetTowerHead();
		const FRotator OriginRotation = Tower->GetActorRotation();
		
		RotateSinTask = UAT_RotateIdleSin::RotateIdleSin(this, TEXT("Rotate"),
			RotationRange, AverageDegree, RotationComp.Get(), OriginRotation);
	}
}

void UKeepWatchPassiveAbility::StartKeepWatch(const TWeakObjectPtr<AEnemyTower> Tower)
{
	if(Tower.IsValid())
	{
		const float TimerRate = Tower->LookForPlayerTimerRate;
		const float MaxRange = Tower->LookForPlayerMaxRange;
		const float FOVRadians = Tower->LookforPlayerFOV;
		const TWeakObjectPtr<USceneComponent> OriginComponent = Tower->GetTowerHead();
		
		FindPlayerTask = UAT_FindPlayer::FindPlayer(this, TEXT("FindPlayer"),
			TimerRate, MaxRange, FOVRadians,OriginComponent.Get());

		if(FindPlayerTask != nullptr)
		{
			FindPlayerTask->PlayerFoundDelegate.AddDynamic(this, &ThisClass::OnPlayerFound);
			FindPlayerTask->PlayerLostDelegate.AddDynamic(this, &ThisClass::OnPlayerLost);
		}
	}
}

void UKeepWatchPassiveAbility::ListenCooldownTagChanged()
{
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = GetAbilitySystemComponentFromActorInfo();
	if(ASC.IsValid())
	{
		const FTanksterGameplayTags& NativeTags = FTanksterGameplayTags::Get();
		CooldownTagChangedHandle = ASC->RegisterGameplayTagEvent(NativeTags.Cooldown_FireProjectile,
		                              EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnFireProjectileCooldownTagChanged);
	}
}

void UKeepWatchPassiveAbility::RemoveCooldownTagChangedListener() const
{
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = GetAbilitySystemComponentFromActorInfo();
	if(ASC.IsValid())
	{
		const FTanksterGameplayTags& NativeTags = FTanksterGameplayTags::Get();
		ASC->UnregisterGameplayTagEvent(CooldownTagChangedHandle, NativeTags.Cooldown_FireProjectile, EGameplayTagEventType::NewOrRemoved);
	}
}

void UKeepWatchPassiveAbility::StartLookAtTargetLocation(const TWeakObjectPtr<APawn> InTargetPawn)
{
	if(LookAtLocationTask != nullptr)
	{
		LookAtLocationTask->SetPause(false);
	}
	
	const TWeakObjectPtr<AEnemyTower> Tower = GetOwningTower();
	if(Tower.IsValid())
	{
		const float TurningDegreePerSecond = Tower->AverageTurningDegreePerSecondIdle;
		const TWeakObjectPtr<USceneComponent> ComponentToRotate = Tower->TowerHead;
		
		
		LookAtLocationTask = UAT_LookAtDynamicLocation::LookAtDynamicLocation(this, TEXT("LookAtLocation"),
			TurningDegreePerSecond, ComponentToRotate.Get(), InTargetPawn.Get(), TowerType, Tower->DistanceToTravelTimeCurve);

		if(LookAtLocationTask != nullptr)
		{
			LookAtLocationTask->LookAtTargetReached.AddDynamic(this, &ThisClass::OnTargetLocationReached);
			ListenCooldownTagChanged();
		}
	}
}

void UKeepWatchPassiveAbility::PauseLookAtTargetLocation()
{
	if(LookAtLocationTask != nullptr)
	{
		LookAtLocationTask->SetPause(true);
	}
}

void UKeepWatchPassiveAbility::OnWaitForPlayerEnds()
{
	if(LookAtLocationTask != nullptr)
	{
		LookAtLocationTask->EndTask();
	}
	WaitForPlayerReturn.Invalidate();
	StartIdleRotation(GetOwningTower());
}

void UKeepWatchPassiveAbility::NotifyTargetForPrediction(
	const TWeakObjectPtr<APawn> TargetPawn)
{
	const TWeakObjectPtr<UAbilitySystemComponent> TargetASC = GetTargetAbilitySystemComponent(TargetPawn);
	if(!TargetASC.IsValid())
	{
		return;
	}
	
	FGameplayTag TagToAdd;
	const FTanksterGameplayTags& NativeTags = FTanksterGameplayTags::Get();
	if(TowerType == ETowerType::FullyPredicted)
	{
		TagToAdd = NativeTags.Target_Predict_All;
	}
	else if (TowerType == ETowerType::LocationPredicted)
	{
		TagToAdd = NativeTags.Target_Predict_Location;
	}
	else
	{
		return;
	}
	
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(UGameplayEffect::StaticClass());
	if(!SpecHandle.IsValid())
	{
		return;
	}
	SpecHandle.Data->DynamicGrantedTags.AddTag(TagToAdd);
	GETargetPredictsHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void UKeepWatchPassiveAbility::NotifyTargetStopPrediction(const TWeakObjectPtr<APawn> TargetPawn) const
{
	const TWeakObjectPtr<UAbilitySystemComponent> TargetASC = GetTargetAbilitySystemComponent(TargetPawn);
	if(!TargetASC.IsValid())
	{
		return;
	}
	TargetASC->RemoveActiveGameplayEffect(GETargetPredictsHandle, 1);
}
