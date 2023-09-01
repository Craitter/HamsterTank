// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TanksterInputConfig.generated.h"


class UInputAction;

USTRUCT(BlueprintType)
struct FTanksterInputAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag = FGameplayTag::EmptyTag;
};

/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTanksterInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UTanksterInputConfig(const FObjectInitializer& ObjectInitializer);
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag,
	                                                bool bLogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTanksterInputAction> NativeInputActions;
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically
	// bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTanksterInputAction> AbilityInputActions;
};
