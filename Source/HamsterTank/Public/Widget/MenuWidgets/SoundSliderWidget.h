// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "SoundSliderWidget.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API USoundSliderWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USlider> SoundSlider = {nullptr};
	
	virtual void NativePreConstruct() override;
protected:
	UFUNCTION()
	void OnSoundSliderValueChanged(float NewValue);
};
