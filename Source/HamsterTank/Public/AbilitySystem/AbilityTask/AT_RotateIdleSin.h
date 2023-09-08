// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_RotateIdleSin.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FOnSinRotationFinishedDelegate);

UCLASS()
class HAMSTERTANK_API UAT_RotateIdleSin : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAT_RotateIdleSin();
	//Has to be stopped manually, will never end by its own!!
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UAT_RotateIdleSin* RotateIdleSin(UGameplayAbility* OwningAbility, FName TaskInstanceName, float InYawRotationRange, float InAverageDegreePerSecond,
	                                USceneComponent* InComponentToRotate, FRotator InOriginRotation);

	float YawRotationRange = 0.0f;
	float AverageDegreePerSecond = 0.0f;
	TWeakObjectPtr<USceneComponent> ComponentToRotate = {nullptr};
	FRotator OriginRotation = FRotator::ZeroRotator;

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	FRotator RotationRangeUpperBound = FRotator::ZeroRotator;
	FRotator RotationRangeLowerBound = FRotator::ZeroRotator;
	
	FRotator SinStartRotation = FRotator::ZeroRotator;
	FRotator SinEndRotation = FRotator::ZeroRotator;

	float CurrentTurningTime = 0.0f;
	float SinDeltaDegree = 0.0f;

	void SetSinStartEndRotation(const FRotator& InStartRotation, const FRotator& InEndRotation);

	FOnSinRotationFinishedDelegate OnSinRotationFinishedDelegate;
	void OnSinRotationFinished();
};
