// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/TanksterGameplayAbility.h"
#include "PredictMovementAbility.generated.h"

class ATankBase;
class UAT_PredictMovement;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UPredictMovementAbility : public UTanksterGameplayAbility
{
	GENERATED_BODY()
public:
	UPredictMovementAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	FVector GetPredictedLocationForTime(const float& Time) const;
protected:
	UPROPERTY()
	float DeltaTime = 0.0f;

	UPROPERTY()
	float SubSteppingRate = 0.0f;

	UPROPERTY()
	float SecondsToPredict = 0.0f;

	TWeakObjectPtr<ATankBase> OwningTank = {nullptr};

	TArray<FVector> PredictedLocations;
	
	
	void OnLocationPredicted(const FVector& PredictedLocation);

	void ResetPredictions();
	
private:
	TObjectPtr<UAT_PredictMovement> PredictMovementTask = {nullptr};	
};
