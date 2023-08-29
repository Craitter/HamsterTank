// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "VictoryWidget.generated.h"

class UCalculatePointsWidget;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UVictoryWidget : public UTankBaseWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;
public:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> VictoryAnimation = {nullptr};
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCalculatePointsWidget> CalculatePointsWidget = {nullptr};

	FWidgetAnimationDynamicEvent Event;
	UFUNCTION()
	void OnVictoryAnimationFinished();
};
