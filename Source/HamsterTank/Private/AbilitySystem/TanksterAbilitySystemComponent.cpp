// Fill out your copyright notice in the Description page of Project Settings.



#include "AbilitySystem/TanksterAbilitySystemComponent.h"

#include "AbilitySystem/TanksterGameplayEffect.h"

UTanksterAbilitySystemComponent::UTanksterAbilitySystemComponent()
{
}

void UTanksterAbilitySystemComponent::InitializeDefaultAttributeValues(
	const TArray<TSubclassOf<UTanksterGameplayEffect>>& DefaultAttributeEffects)
{
	
	if(DefaultAttributeEffects.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in Blueprint of %s."), *FString(__FUNCTION__), *GetName(), GetAvatarActor() ? *GetAvatarActor()->GetName() : TEXT("AvatarActorWasNull"));
		return;
	}
	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	for(const TSubclassOf<UTanksterGameplayEffect> AttributesToAdd : DefaultAttributeEffects)
	{
		const FGameplayEffectSpecHandle TempOutSpecHandle = MakeOutgoingSpec(AttributesToAdd, 1, EffectContextHandle);
		if(TempOutSpecHandle.IsValid())
		{
			// FActiveGameplayEffectHandle ActiveGEHandle =
			ApplyGameplayEffectSpecToTarget(*TempOutSpecHandle.Data.Get(), this);
		}
	}
}
