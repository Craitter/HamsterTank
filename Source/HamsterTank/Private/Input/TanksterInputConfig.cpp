// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/TanksterInputConfig.h"

UTanksterInputConfig::UTanksterInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UTanksterInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag,
                                                                      bool bLogNotFound) const
{
	for (const FTanksterInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UTanksterInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
                                                                       bool bLogNotFound) const
{
	for (const FTanksterInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
