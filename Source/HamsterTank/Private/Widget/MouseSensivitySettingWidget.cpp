// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MouseSensivitySettingWidget.h"

#include "TankPlayerController.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Widget/UISubsystem.h"

void UMouseSensivitySettingWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(UISubsystem.IsValid())
	{
		UISubsystem->PauseGameDelegate.AddUObject(this, &ThisClass::LoadMouseSensivity);
		if(MouseSensivitySlider != nullptr)
		{
			MouseSensivitySlider->SetMaxValue(10.0f);
			MouseSensivitySlider->SetMinValue(0.1f);
			MouseSensivitySlider->SetStepSize(0.1f);
						
			MouseSensivitySlider->OnValueChanged.AddDynamic(this, &ThisClass::OnMouseSensivitySliderValueChanged);
		}
	}
}

void UMouseSensivitySettingWidget::OnMouseSensivitySliderValueChanged(float NewValue)
{
	if(GetOwningPlayer() != nullptr)
	{
		const TWeakObjectPtr<ATankPlayerController> TankPlayerController = Cast<ATankPlayerController>(GetOwningPlayer());
		if(TankPlayerController.IsValid())
		{
			TankPlayerController->SetMouseSensitivity(NewValue);
			
		}
		if(Value != nullptr)
		{
			FNumberFormattingOptions Options;
			Options.MaximumFractionalDigits = 1;
			Options.MinimumFractionalDigits = 1;
			Value->SetText(FText::AsNumber(NewValue, &Options));
		}
	}
}

void UMouseSensivitySettingWidget::LoadMouseSensivity()
{
	if(GetOwningPlayer() != nullptr && MouseSensivitySlider != nullptr)
	{
		const TWeakObjectPtr<ATankPlayerController> TankPlayerController = Cast<ATankPlayerController>(GetOwningPlayer());
		if(TankPlayerController.IsValid())
		{
			if(Value != nullptr)
			{
				FNumberFormattingOptions Options;
				Options.MaximumFractionalDigits = 1.0f;
				Value->SetText(FText::AsNumber(TankPlayerController->GetMouseSens(), &Options));
			}
			MouseSensivitySlider->SetValue(TankPlayerController->GetMouseSens());
		}
	}
}
