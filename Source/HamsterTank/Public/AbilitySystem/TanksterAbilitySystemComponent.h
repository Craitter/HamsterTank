// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TanksterAbilitySystemComponent.generated.h"

class UTanksterGameplayEffect;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTanksterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UTanksterAbilitySystemComponent();

	void InitializeDefaultAttributeValues(const TArray<TSubclassOf<UTanksterGameplayEffect>>& DefaultAttributeEffects);
};


