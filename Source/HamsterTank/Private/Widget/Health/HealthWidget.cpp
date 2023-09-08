// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Health/HealthWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "Components/DynamicEntryBox.h"
#include "GameClasses/Player/TanksterPlayerState.h"
#include "Widget/Health/HealthEntryWidget.h"

bool UHealthWidget::Initialize()
{
	if(!Super::Initialize()) return false;

	const TWeakObjectPtr<ATanksterPlayerState> PlayerState = GetOwningPlayerState<ATanksterPlayerState>();
	if(PlayerState.IsValid())
	{
		const TWeakObjectPtr<UHealthAttributeSet> HealthAttributeSet = PlayerState->GetHealthAttributeSet();
		const TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		if(AbilitySystemComponent.IsValid() && HealthAttributeSet.IsValid())
		{
			UpdateMaxHealth(PlayerState->GetMaxHealth());
			UpdateHealth(PlayerState->GetHealth());
			HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
			MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		}
	}
	return true;
}

void UHealthWidget::NativeDestruct()
{
	const TWeakObjectPtr<ATanksterPlayerState> PlayerState = GetOwningPlayerState<ATanksterPlayerState>();
	if(PlayerState.IsValid())
	{
		const TWeakObjectPtr<UHealthAttributeSet> HealthAttributeSet = PlayerState->GetHealthAttributeSet();
		const TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		if(AbilitySystemComponent.IsValid() && HealthAttributeSet.IsValid())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetMaxHealthAttribute()).Remove(MaxHealthChangedDelegateHandle);
			HealthChangedDelegateHandle.Reset();
			MaxHealthChangedDelegateHandle.Reset();
		}
	}
	Super::NativeDestruct();
}

void UHealthWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	
	UpdateHealth(FMath::RoundToInt32(Data.NewValue));
	
}

void UHealthWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdateMaxHealth(FMath::RoundToInt32(Data.NewValue));
}

void UHealthWidget::UpdateMaxHealth(const int32 NewMaxHealth)
{
	if(HealthIcons == nullptr)
	{
		return;
	}
	HealthIcons->Reset();
	HealthWidgets.Empty();
	
	int32 Index = 0;
	do
	{
		Index += 2;
		TObjectPtr<UHealthEntryWidget> NewWidget = HealthIcons->CreateEntry<UHealthEntryWidget>();
		if(NewWidget != nullptr)
		{
			NewWidget->SetHealthState(EHealthState::Empty);
			HealthWidgets.Insert(NewWidget, 0);
		}
	}
	while (Index < NewMaxHealth);
}

void UHealthWidget::UpdateHealth(int32 NewHealth)
{
	if(HealthIcons == nullptr)
	{
		return;
	}
	int Index = 0;
	
	for (const auto Element : HealthWidgets)
	{
		
		Index++;
		if(Element == nullptr)
		{
			Index++;
			continue;
		}
		
		if(Index <= NewHealth)
		{
			if(Index + 1 > NewHealth)
			{
				
				Element->SetHealthState(EHealthState::Half);
			}
			else
			{
				
				Element->SetHealthState(EHealthState::Full);
			}
		}
		else
		{
			
			Element->SetHealthState(EHealthState::Empty);
		}
		Index ++;
	}
}
