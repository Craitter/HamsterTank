// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "PauseMenuWidget.generated.h"

class UGenericSliderWidget;
class UOverlayButtonsWidget;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UPauseMenuWidget : public UTankBaseWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlayButtonsWidget> Buttons = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGenericSliderWidget> SoundSlider = {nullptr};
	
};
