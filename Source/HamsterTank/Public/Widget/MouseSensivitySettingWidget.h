// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "MouseSensivitySettingWidget.generated.h"

class UTextBlock;
class USlider;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UMouseSensivitySettingWidget : public UTankBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USlider> MouseSensivitySlider = {nullptr};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> Value = {nullptr};
	
	virtual void NativePreConstruct() override;
protected:
	UFUNCTION()
	void OnMouseSensivitySliderValueChanged(float NewValue);

	void LoadMouseSensivity();
};
