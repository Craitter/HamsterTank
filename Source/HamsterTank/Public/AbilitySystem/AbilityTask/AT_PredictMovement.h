// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_PredictMovement.generated.h"

class UTankMovementComponent;
class ATankBase;
/**
 * 
 */

DECLARE_DELEGATE_OneParam(FOnMovementPredicted, const FVector&);
DECLARE_DELEGATE(FOnPredictionStarted);

UCLASS()
class HAMSTERTANK_API UAT_PredictMovement : public UAbilityTask
{
	GENERATED_BODY()

public:
	static UAT_PredictMovement* PredictMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName, ATankBase* InOwningTank, const bool
	                                            bShouldPredictRotation, const float& InDeltaTime, const float& InSubSteppingRate, const float& InSecondsToPredict);

	virtual void Activate() override;


	float GetDeltaTime() const;
	
	TWeakObjectPtr<ATankBase> OwningTank = {nullptr};

	bool bPredictRotation = false;

	float SecondsToPredict = 3.0f;
	
	float DeltaTime = 0.0f;

	float SubSteppingRate = 0.05;

	FOnMovementPredicted OnMovementPredicted;
	FOnPredictionStarted OnPredictionStarted;
protected:
	void PredictMovement();
private:
	FTimerHandle PredictHandle;

	TWeakObjectPtr<UTankMovementComponent> MovementComponent = {nullptr};
};
