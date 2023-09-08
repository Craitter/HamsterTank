// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbilities/PredictMovementAbility.h"

#include "AbilitySystem/AbilityTask/AT_PredictMovement.h"
#include "Actors/TankBase.h"
#include "GameClasses/TanksterGameplayTags.h"

UPredictMovementAbility::UPredictMovementAbility()
{
	ActivationPolicy = ETanksterAbilityActivationPolicy::ByEvent;

	
}

void UPredictMovementAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if(TriggerEventData == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
	const FTanksterGameplayTags& NativeTags = FTanksterGameplayTags::Get();
	const bool bPredictRotation = TriggerEventData->EventTag == NativeTags.Target_Predict_All;
	
	PredictMovementTask = UAT_PredictMovement::PredictMovement(this, TEXT("PredictMovement"),
		Cast<ATankBase>(GetAvatarActorFromActorInfo()), bPredictRotation, DeltaTime, SubSteppingRate, SecondsToPredict);
	if(PredictMovementTask == nullptr)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
	
	PredictMovementTask->OnMovementPredicted.BindUObject(this, &ThisClass::OnLocationPredicted);
	PredictMovementTask->OnPredictionStarted.BindUObject(this, &ThisClass::ResetPredictions);
	ResetPredictions();

	
}

void UPredictMovementAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FVector UPredictMovementAbility::GetPredictedLocationForTime(const float& Time) const
{
	if(!OwningTank.IsValid())
	{
		return FVector::ZeroVector;
	}
	
	if(PredictMovementTask == nullptr || PredictedLocations.IsEmpty())
	{
		return OwningTank->GetActorLocation();
	}
	const float TotalTime = FMath::Clamp<float>(Time, 0.0f, SecondsToPredict) + PredictMovementTask->GetDeltaTime();
	const int32 Index = FMath::FloorToInt32(TotalTime / SubSteppingRate);
	if(PredictedLocations.IsValidIndex(Index))
	{
		return PredictedLocations[Index];
	}
	return OwningTank->GetActorLocation();
}

void UPredictMovementAbility::OnLocationPredicted(const FVector& PredictedLocation)
{
	PredictedLocations.Add(PredictedLocation);
}

void UPredictMovementAbility::ResetPredictions()
{
	PredictedLocations.Empty();
	PredictedLocations.Reserve(SecondsToPredict / SubSteppingRate);
}
