// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupData.generated.h"

/**
 * 
 */

class UTanksterGameplayEffect;
class ATankBase;
class UNiagaraSystem;
class USoundCue;

UENUM(BlueprintType)
enum EPickupType : uint8
{
	None = 0,

	Nothing = 1,
	
	Heal = 2,

	Ammo = 3,

	Cherry = 4,
};

USTRUCT(BlueprintType)
struct FPickupData_Backend
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPickupType> Type = None;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> DefaultNiagara = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> PickupNiagara = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> PickupSound = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTanksterGameplayEffect> PickupGameplayEffect= {nullptr};
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ForceUnits = "Units"))
	float WeightedProbability = 1.0f; 

	
};

USTRUCT(BlueprintType)
struct FForcePickup
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0", ForceUnits = "Units"))
	float MinValue = 1.0f;

	//Higher Number Higher Prio
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0", ForceUnits = "Units"))
	int32 Priority = 0;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EPickupType> Type = None;

	bool operator < (const FForcePickup Pickup) const
	{
		return Priority < Pickup.Priority;
	}
};

UCLASS()
class HAMSTERTANK_API UPickupData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPickupData();
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FPickupData_Backend> Pickups;

	//To disable a force of a pickup set its min value to 0;
	UPROPERTY(EditDefaultsOnly, Category = "ForcePickup")
	TArray<FForcePickup> ForcePickups;
private:
	float CalculateSum();
	EPickupType DoesPlayerNeedResources();

	static bool NeedsAmmo(TWeakObjectPtr<ATankBase> PlayerTank);
	static bool NeedsHealth(TWeakObjectPtr<ATankBase> PlayerTank);
	
public:
	FPickupData_Backend* ChoosePickup();
};
