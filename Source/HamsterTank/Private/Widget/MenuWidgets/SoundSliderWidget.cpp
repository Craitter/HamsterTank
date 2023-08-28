// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/MenuWidgets/SoundSliderWidget.h"

#include "TankHamsterGameInstance.h"
#include "Components/Slider.h"
#include "Widget/UISubsystem.h"

void USoundSliderWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(UISubsystem.IsValid())
	{
		if(SoundSlider != nullptr)
		{
			SoundSlider->OnValueChanged.AddDynamic(this, &ThisClass::OnSoundSliderValueChanged);
			const float SoundPercentage = UISubsystem->GetSoundPercentage();
			SoundSlider->SetValue(SoundPercentage);
		}
	}
}

void USoundSliderWidget::OnSoundSliderValueChanged(float NewValue)
{
	if(GameInstance.IsValid())
	{
		GameInstance->SetSoundPercentage(NewValue);
	}
}
