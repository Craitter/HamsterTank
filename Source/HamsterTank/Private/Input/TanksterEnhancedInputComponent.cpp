// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/TanksterEnhancedInputComponent.h"



UTanksterEnhancedInputComponent::UTanksterEnhancedInputComponent()
{
}





inline void UTanksterEnhancedInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
