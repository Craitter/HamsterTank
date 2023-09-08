// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Actors/TankBase.h"
#include "AT_LookAtDynamicLocation.generated.h"

/**
 * 
 */

class AEnemyTower;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLookAtTargetReached, const float&, Distance);

UCLASS()
class HAMSTERTANK_API UAT_LookAtDynamicLocation : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAT_LookAtDynamicLocation();
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UAT_LookAtDynamicLocation* LookAtDynamicLocation(UGameplayAbility* OwningAbility,
	                                                        const FName TaskInstanceName, float InTurningDegreePerSecond, USceneComponent* InComponentToRotate, APawn* InTargetPawn, ETowerType
	                                                        InTowerType, const FRuntimeFloatCurve& InDistanceSpeedCurve);


	virtual void Activate() override;
	bool GetTargetLocation(FVector& TargetLocation);

	virtual void TickTask(float DeltaTime) override;

	void SetIsOnCooldown(bool bIsOnCooldown);
	
	
	float TurningDegreePerSecond = 0.0f;

	TWeakObjectPtr<USceneComponent> ComponentToRotate = {nullptr};

	TWeakObjectPtr<ATankBase> TargetPawn = {nullptr};

	const FRichCurve* DistanceSpeedCurve = {nullptr};

	FLookAtTargetReached LookAtTargetReached;

	ETowerType TowerType;

	void SetPause(bool bPause);
protected:
	bool bIsOnCooldown = false;

	bool GetIsPaused() const;

private:
	bool bIsPaused = false;
	
};
