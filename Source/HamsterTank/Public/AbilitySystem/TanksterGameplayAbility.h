// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "TanksterGameplayAbility.generated.h"

/**
 * ETanksterAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class ETanksterAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn,

	ByEvent
};
UCLASS()
class HAMSTERTANK_API UTanksterGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UTanksterGameplayAbility();

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
public:

	ETanksterAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tankster|Ability Cost")
	float Cost = 0.0f;
	
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tankster|Ability Activation")
	ETanksterAbilityActivationPolicy ActivationPolicy;
};
