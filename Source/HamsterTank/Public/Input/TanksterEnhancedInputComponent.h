// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "TanksterInputConfig.h"
#include "TanksterEnhancedInputComponent.generated.h"

class UTanksterInputConfig;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTanksterEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UTanksterEnhancedInputComponent();

	template<class UserClass, typename FuncType>
	void BindNativeAction(const TWeakObjectPtr<UTanksterInputConfig> InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound, TArray<uint32>& BindHandles);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const TWeakObjectPtr<UTanksterInputConfig> InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UTanksterEnhancedInputComponent::BindNativeAction(const TWeakObjectPtr<UTanksterInputConfig> InputConfig,
	const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound, TArray<uint32>& BindHandles)
{
	check(InputConfig.Get());
	const UInputAction* ActionToBind = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound);
	if (ActionToBind)
	{
		BindHandles.Add(BindAction(ActionToBind, TriggerEvent, Object, Func).GetHandle());
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UTanksterEnhancedInputComponent::BindAbilityActions(const TWeakObjectPtr<UTanksterInputConfig> InputConfig,
	UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig.Get());

	for (const FTanksterInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}

