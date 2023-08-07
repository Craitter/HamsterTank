// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBaseWidget.h"
#include "MainMenuWidget.generated.h"

class ULeaderboardWidget;
class USoundSliderWidget;
class UScrollBox;
class UWidgetSwitcher;
class USlider;
class UButton;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UMainMenuWidget : public UTankBaseWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	virtual void NativePreConstruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PlayButton = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeaderboardButton = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DesktopButton = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USoundSliderWidget> SoundSlider = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULeaderboardWidget> LeaderboardScrollBox = {nullptr};

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SwapToAchievements = {nullptr};

private:
	UFUNCTION()
	void OnPlayButtonPressed();
	
	UFUNCTION()
	void OnLeaderboardButtonPressed();
	
	UFUNCTION()
	void OnDesktopButtonPressed();
	
	
	UFUNCTION()
	void OnBackButtonPressed();

};
