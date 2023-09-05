// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ProjectileOriginComponent.generated.h"

USTRUCT(BlueprintType)
struct FFireProjectileData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Spread")
	bool bApplySpread = false;
	
	UPROPERTY(EditAnywhere, Category = "Spread")
	float YawSpreadDegree = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	bool bApplyCooldown = false;
	
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	bool bRandomizeCooldown = false;
	
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float MinCooldown = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float MaxCooldown = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float FireCooldown = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Speed")
	bool bApplyCustomSpeed = false;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float CustomSpeed = -1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	bool bRandomizeSpeed = false;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float InMinSpeedModifier = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float InMaxSpeedModifier = 1.0f;
	
	
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAMSTERTANK_API UProjectileOriginComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileOriginComponent();

	// UPROPERTY(BlueprintCallable)
	// FFireProjectileData GetProjectileData();
	
	FFireProjectileData* GetProjectileData();
protected:
	UPROPERTY(EditAnywhere, Category = "ProjectileData")
	FFireProjectileData FireProjectileData;
};
