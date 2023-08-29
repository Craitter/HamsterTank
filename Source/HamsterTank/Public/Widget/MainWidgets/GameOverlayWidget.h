// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/TankBaseWidget.h"
#include "GameOverlayWidget.generated.h"


/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UGameOverlayWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:

	virtual bool Initialize() override;
	
	void DisplayKill();
protected:
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DisplayKillAnimation = {nullptr};

private:
};
