// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TankBaseWidget.generated.h"

class UOverlay;
class UUISubsystem;
class UPromptWidget;
class UMenuAnchor;
class UTankHamsterGameInstance;
// class UUISubsystem;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTankBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	void EnablePrompt(TWeakObjectPtr<UPromptWidget> PromptToDisplay) const;
	void DisablePrompt();
protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UMenuAnchor> PromptAnchor = {nullptr};

	
	TWeakObjectPtr<UUISubsystem> UISubsystem = {nullptr};

	TWeakObjectPtr<UTankHamsterGameInstance> GameInstance = {nullptr};
};
