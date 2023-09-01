// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Ammo/AmmoWidget.h"

#include "AbilitySystem/AttributeSets/AmmoAttributeSet.h"
#include "Components/TextBlock.h"
#include "GameClasses/Player/TanksterPlayerState.h"

bool UAmmoWidget::Initialize()
{
	if(!Super::Initialize()) return false;

	const TWeakObjectPtr<ATanksterPlayerState> PlayerState = GetOwningPlayerState<ATanksterPlayerState>();
	if(PlayerState.IsValid())
	{
		const TWeakObjectPtr<UAmmoAttributeSet> AmmoAttributeSet = PlayerState->GetAmmoAttributeSet();
		const TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		if(AbilitySystemComponent.IsValid() && AmmoAttributeSet.IsValid())
		{
			UpdateMaxAmmo(PlayerState->GetMaxAmmo());
			UpdateAmmo(PlayerState->GetAmmo());
			AmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetAmmoAttribute()).AddUObject(this, &ThisClass::OnAmmoChanged);
			MaxAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetMaxAmmoAttribute()).AddUObject(this, &ThisClass::OnMaxAmmoChanged);
		}
	}
	NumberFormattingOptions.AlwaysSign = false;
	NumberFormattingOptions.MaximumFractionalDigits = 0;
	NumberFormattingOptions.MinimumFractionalDigits = 0;
	NumberFormattingOptions.MaximumIntegralDigits = 2;
	NumberFormattingOptions.MinimumIntegralDigits = 2;

	return true;
}

void UAmmoWidget::NativeDestruct()
{

	const TWeakObjectPtr<ATanksterPlayerState> PlayerState = GetOwningPlayerState<ATanksterPlayerState>();
	if(PlayerState.IsValid())
	{
		const TWeakObjectPtr<UAmmoAttributeSet> HealthAttributeSet = PlayerState->GetAmmoAttributeSet();
		const TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		if(AbilitySystemComponent.IsValid() && HealthAttributeSet.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetAmmoAttribute()).Remove(AmmoChangedDelegateHandle);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetMaxAmmoAttribute()).Remove(MaxAmmoChangedDelegateHandle);
			AmmoChangedDelegateHandle.Reset();
			MaxAmmoChangedDelegateHandle.Reset();
		}
	}
	
	Super::NativeDestruct();
}

void UAmmoWidget::OnAmmoChanged(const FOnAttributeChangeData& Data)
{
	UpdateAmmo(FMath::RoundToInt32(Data.NewValue));
}

void UAmmoWidget::OnMaxAmmoChanged(const FOnAttributeChangeData& Data)
{
	UpdateMaxAmmo(FMath::RoundToInt32(Data.NewValue));
}

void UAmmoWidget::UpdateAmmo(const int32 CurrentAmmo) const
{
	if(AmmoCount != nullptr)
	{
		const FText Count = FText::FromString("{Amount}x");
		
		FFormatNamedArguments Args;
		Args.Add(TEXT("Amount"), CurrentAmmo);
		
		AmmoCount->SetText(FText::Format(Count, Args));
	}
}

void UAmmoWidget::UpdateMaxAmmo(int32 CurrentMaxAmmo)
{
}
