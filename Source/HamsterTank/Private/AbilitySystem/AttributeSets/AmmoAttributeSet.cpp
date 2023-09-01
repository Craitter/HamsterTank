// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSets/AmmoAttributeSet.h"

#include "GameplayEffectExtension.h"

void UAmmoAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetMaxAmmoAttribute())
	{
		AdjustAttributeForMaxChange(Ammo,MaxAmmo, NewValue, GetAmmoAttribute());
	}

	else if(Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmo());
	}
}

void UAmmoAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if(Data.EvaluatedData.Attribute == GetAmmoAttribute())
	{
		SetAmmo(FMath::Clamp(GetAmmo(), 0.0f, GetMaxAmmo()));
	}
}

void UAmmoAttributeSet::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
                                                    const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
                                                    const FGameplayAttribute& AffectedAttributeProperty) const
{
	const TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if(!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilitySystem.IsValid())
	{
		if(CurrentMaxValue > NewMaxValue)
		{
			if(NewMaxValue > AffectedAttribute.GetCurrentValue())
			{
				return;
			}
			else
			{
				const float AffectedDelta = NewMaxValue - AffectedAttribute.GetCurrentValue();
				AbilitySystem->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, AffectedDelta);
			}
		}
		else
		{
			const float AbsDeltaMaxValue = NewMaxValue - CurrentMaxValue;
			AbilitySystem->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, AbsDeltaMaxValue);
		}
	}
}
