// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameClasses/TanksterGameplayTags.h"
#include "DamageEffectExecutionCalculation.generated.h"

/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UDamageEffectExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UDamageEffectExecutionCalculation();
	bool DoesHitDirectionMatter(TWeakObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent,
	                            const FTanksterGameplayTags& NativeTags) const;

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	float SideHitMultiplier = 1.0f;
};
