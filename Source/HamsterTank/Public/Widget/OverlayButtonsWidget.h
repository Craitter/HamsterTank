// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "OverlayButtonsWidget.generated.h"

enum class EPromptReply : uint8;
class UButton;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UOverlayButtonsWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HomeButton = {nullptr};

public:
	void ShowResumeButton(bool bShowResume) const;
private:
	UFUNCTION()
	void OnResumePressed();
	UFUNCTION()
	void OnRestartPressed();
	UFUNCTION()
	void OnHomePressed();
};
