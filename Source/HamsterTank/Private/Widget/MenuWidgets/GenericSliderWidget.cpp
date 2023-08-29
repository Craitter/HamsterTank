// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/MenuWidgets/GenericSliderWidget.h"

#include "TankHamsterGameInstance.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

bool UGenericSliderWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	
	return true;
}

void UGenericSliderWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(!ensure(NameTextBlock != nullptr)) return;
	if(!ensure(Slider != nullptr)) return;
	
	ValueFormatOptions.MaximumFractionalDigits = MaxFractionalNum;
	ValueFormatOptions.MinimumFractionalDigits = MinFractionalNum;
	ValueFormatOptions.MaximumIntegralDigits = MaxIntegralNum;
	ValueFormatOptions.MinimumIntegralDigits = MinIntegralNum;
	
	Slider->SetMinValue(MinValue);
	Slider->SetMaxValue(MaxValue);
	Slider->OnValueChanged.AddDynamic(this, &ThisClass::UGenericSliderWidget::OnValueChanged);
	const float CurrentValue = LoadCurrentValue();
	Slider->SetValue(CurrentValue);
	OnValueChanged(CurrentValue);
	
	
	NameTextBlock->SetText(SliderName);
}


float UGenericSliderWidget::LoadCurrentValue()
{
	if(GameInstance.IsValid())
	{
		const float OutValue = GameInstance->GetSliderValue(SliderType);
		return OutValue < 0 ? MaxValue : OutValue; 
	}
	return -1.0f;
}

void UGenericSliderWidget::HandleValueChanged(const float NewValue)
{
	if(GameInstance.IsValid())
	{
		GameInstance->SetSliderValue(NewValue, SliderType);
	}
}

void UGenericSliderWidget::OnValueChanged(float NewValue)
{
	
	UpdateDisplayedValue(NewValue);
	HandleValueChanged(NewValue);
}

void UGenericSliderWidget::UpdateDisplayedValue(const float NewValue) const
{
	if(ValueTextBlock != nullptr)
	{
		ValueTextBlock->SetText(FText::AsNumber(NewValue, &ValueFormatOptions));
	}
}
