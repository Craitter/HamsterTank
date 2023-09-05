// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/TanksterAbilitySet.h"

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystem/TanksterAbilitySystemComponent.h"
#include "AbilitySystem/TanksterGameplayAbility.h"
#include "AbilitySystem/TanksterGameplayEffect.h"

void FTanksterAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FTanksterAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FTanksterAbilitySet_GrantedHandles::TakeFromAbilitySystem(
	TWeakObjectPtr<UTanksterAbilitySystemComponent> TanksterASC)
{
	check(TanksterASC.Get())

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			TanksterASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			TanksterASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
}

UTanksterAbilitySet::UTanksterAbilitySet(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UTanksterAbilitySet::GiveToAbilitySystem(UTanksterAbilitySystemComponent* TanksterASC,
	FTanksterAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FTanksterAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			// UE_LOG(LogLyraAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		TObjectPtr<UTanksterGameplayAbility> AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UTanksterGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = TanksterASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FTanksterAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			// UE_LOG(LogLyraAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const TObjectPtr<UTanksterGameplayEffect> GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UTanksterGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = TanksterASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, TanksterASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}
