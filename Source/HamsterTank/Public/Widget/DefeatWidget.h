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
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlayButtonsWidget> Buttons = {nullptr};
};
