// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "InstructionsWidget.generated.h"

class AHamsterTankGameModeBase;
class UButton;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UInstructionsWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ContinueAnimation = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InstructionsButton = {nullptr};

	FWidgetAnimationDynamicEvent StartGameAnimationEvent;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartGameAnimation = {nullptr};

	UFUNCTION()
	void OnStartGameAnimationFinished();

	UFUNCTION()
	void OnInstructionsRead();
	
private:
	TWeakObjectPtr<AHamsterTankGameModeBase> HamsterTankGameModeBase = {nullptr};
};
