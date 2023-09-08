// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Calculations/DamageEffectExecutionCalculation.h"

#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "GameClasses/TanksterGameplayTags.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct TanksterDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	TanksterDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// We're not capturing anything from the Source in this example, but there could be like AttackPower attributes that you might want.

		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHealthAttributeSet, Damage, Source, true);
	}
};

static const TanksterDamageStatics& DamageStatics()
{
	static TanksterDamageStatics DStatics;
	return DStatics;
}

UDamageEffectExecutionCalculation::UDamageEffectExecutionCalculation()
{
	SideHitMultiplier = 2.0f;

	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

bool UDamageEffectExecutionCalculation::DoesHitDirectionMatter(
	TWeakObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent, const FTanksterGameplayTags& NativeTags) const
{
	return TargetAbilitySystemComponent.IsValid() && !TargetAbilitySystemComponent->HasMatchingGameplayTag(
		NativeTags.Hit_DirectionImmune);
}

void UDamageEffectExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TWeakObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	TWeakObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	TWeakObjectPtr<AActor> SourceActor = SourceAbilitySystemComponent.IsValid() ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	TWeakObjectPtr<AActor> TargetActor = TargetAbilitySystemComponent.IsValid() ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;

	const FTanksterGameplayTags& NativeTags = FTanksterGameplayTags::Get();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();



	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;


	
	float Damage = 0.0f;

	if(TargetTags == nullptr || !TargetTags->HasTagExact(NativeTags.Immortal))
	{
		// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
		// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
		// Add SetByCaller damage if it exists
		Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(NativeTags.Damage, false, -1.0f), 0.0f);
	}
	

	FGameplayTagContainer AssetTags;
	Spec.GetAllAssetTags(AssetTags);
	
	if(DoesHitDirectionMatter(TargetAbilitySystemComponent, NativeTags))
	{
		FGameplayEffectSpec* MutableSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
		const FHitResult* Result = Spec.GetEffectContext().GetHitResult();
		if(TargetActor.IsValid() && Result != nullptr && MutableSpec != nullptr)
		{
			
			const FVector& ActorLocation = TargetActor->GetActorLocation();
			// PointPlaneDist is super cheap - 1 vector subtraction, 1 dot product.
			float DistanceToFrontBackPlane = FVector::PointPlaneDist(Result->Location, ActorLocation, TargetActor->GetActorRightVector());
			float DistanceToRightLeftPlane = FVector::PointPlaneDist(Result->Location, ActorLocation, TargetActor->GetActorForwardVector());

			FGameplayTag HitDirectionToAdd;
			if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
			{
				// Determine if Front or Back
				HitDirectionToAdd = DistanceToRightLeftPlane >= 0 ? NativeTags.Hit_Front : NativeTags.Hit_Back;
				// Can see if it's left or right of Left/Right plane which would determine Front or Back
			}
			else
			{
				// Determine if Right or Left
				HitDirectionToAdd = DistanceToFrontBackPlane >= 0 ? NativeTags.Hit_Right : NativeTags.Hit_Left;

				Damage *= SideHitMultiplier;
			}
			MutableSpec->AddDynamicAssetTag(HitDirectionToAdd);
		}		
	}
	


	


	
	if (Damage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, Damage));
	}
}
