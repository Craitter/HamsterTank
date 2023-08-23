// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "DefeatWidget.generated.h"

class UOverlayButtonsWidget;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UDefeatWidget : public UTankBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	void PlayDefeatAnimation();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlayButtonsWidget> Buttons = {nullptr};
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LoseAnimation = {nullptr};
};
