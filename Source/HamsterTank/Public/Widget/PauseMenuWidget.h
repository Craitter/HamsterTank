// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class USoundSliderWidget;
class UOverlayButtonsWidget;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlayButtonsWidget> Buttons = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USoundSliderWidget> SoundSlider = {nullptr};
	
};
