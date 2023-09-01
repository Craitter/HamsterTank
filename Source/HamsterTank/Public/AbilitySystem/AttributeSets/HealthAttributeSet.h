// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HealthAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	//Clamping and other changes that might be mandatory for the Attribute just before it gets applied ...
	//anything that recalculates the CurrentValue from all of the modifiers like
	//GameplayEffectExecutionCalculations and ModifierMagnitudeCalculations need to implement clamping again.
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	//Base Value changes of Instant Gameplay Effects should be adjusted right here
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	//Used for example: Only strongest slow effect applies
	// virtual void OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const override;


	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Damage)

protected:
	//Adjusts an Attribute to fit the Percentage it had before the MaxAttribute change
	void AdjustAttributeForMaxChange(const FGameplayAttributeData & AffectedAttribute, const FGameplayAttributeData & MaxAttribute, float NewMaxValue, const FGameplayAttribute & AffectedAttributeProperty) const;
	
};
