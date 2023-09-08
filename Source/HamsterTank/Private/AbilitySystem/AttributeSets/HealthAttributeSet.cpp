// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Actors/TankBase.h"
#include "GameClasses/TanksterGameplayTags.h"

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health,MaxHealth, NewValue, GetHealthAttribute());
	}

	else if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	TWeakObjectPtr<UAbilitySystemComponent> OwnerASC = GetOwningAbilitySystemComponent();
	TWeakObjectPtr<ATankBase> OwnerTank = Cast<ATankBase>(GetOwningActor());
	
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}

	else if(Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		SetDamage(0.0f);
		if(LocalDamage > 0.0f)
		{
			SetHealth(FMath::Clamp(GetHealth() - LocalDamage, 0.0f, GetMaxHealth()));
		
			const FTanksterGameplayTags& NativeTags = FTanksterGameplayTags::Get();
			if(OwnerTank.IsValid())
			{
				FVector2D OutDirection;
				for (auto& Element : Data.EffectSpec.GetDynamicAssetTags())
				{
					
					if(NativeTags.Hit_Back == Element)
					{
						OutDirection = {-1.0f, 0.0f};
					}
					else if(NativeTags.Hit_Back == Element)
					{
						OutDirection = {1.0f, 0.0f};
					}
					else if(NativeTags.Hit_Back == Element)
					{
						OutDirection = {0.0f, -1.0f};
					}
					else if(NativeTags.Hit_Back == Element)
					{
						OutDirection = {0.0f, 1.0f};
					}
					if(!OutDirection.IsZero())
					{
						break;
					}
				}
				OwnerTank->OnHitDirection.Broadcast(OutDirection);
			}
			
			
			// FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
			// UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
			// const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			// FGameplayTagContainer SpecAssetTags;
			// Data.EffectSpec.GetAllAssetTags(SpecAssetTags);
			//
			// // Get the Target actor, which should be our owner
			// AActor* TargetActor = nullptr;
			// AController* TargetController = nullptr;
			// ATankBase* TargetCharacter = nullptr;
			// if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
			// {
			// 	TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			// 	TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
			// 	TargetCharacter = Cast<ATankBase>(TargetActor);
			// }
			//
			// // Get the Source actor
			// AActor* SourceActor = nullptr;
			// AController* SourceController = nullptr;
			// ATankBase* SourceCharacter = nullptr;
			// if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
			// {
			// 	SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			// 	SourceController = Source->AbilityActorInfo->PlayerController.Get();
			// 	if (SourceController == nullptr && SourceActor != nullptr)
			// 	{
			// 		if (APawn* Pawn = Cast<APawn>(SourceActor))
			// 		{
			// 			SourceController = Pawn->GetController();
			// 		}
			// 	}
			//
			// 	// Use the controller to find the source pawn
			// 	if (SourceController)
			// 	{
			// 		SourceCharacter = Cast<ATankBase>(SourceController->GetPawn());
			// 	}
			// 	else
			// 	{
			// 		SourceCharacter = Cast<ATankBase>(SourceActor);
			// 	}
			//
			// 	// Set the causer actor based on context if it's set
			// 	if (Context.GetEffectCauser())
			// 	{
			// 		SourceActor = Context.GetEffectCauser();
			// 	}
			// }
		}
	}
}

void UHealthAttributeSet::AdjustAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
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
