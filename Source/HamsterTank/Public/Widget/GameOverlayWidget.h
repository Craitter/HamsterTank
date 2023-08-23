// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBaseWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlayWidget.generated.h"

class USoundCue;
class UButton;
class UMenuAnchor;
class UPauseMenuWidget;
class UCalculatePointsWidget;
class UVictoryWidget;
class UDefeatWidget;
class UImage;
class UWidgetSwitcher;
class UTextBlock;
class UHealthBarWidget;
class UBulletIconWidget;
class UDynamicEntryBox;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UGameOverlayWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:

	virtual bool Initialize() override;
	
	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnScoreChanged(float NewScore);

	
	
	void DisplayKill();
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBulletIconWidget> Ammo = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHealthBarWidget> HealthBar = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayTime = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InGameScore = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPauseMenuWidget> PauseMenu = {nullptr};
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDefeatWidget> Defeat = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVictoryWidget> Victory = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCalculatePointsWidget> CalculatePoints = {nullptr};

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DisplayKillAnimation = {nullptr};

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartGameAnimation = {nullptr};

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ContinueAnimation = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InstructionsButton = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> DefeatSound = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> VictorySound = {nullptr};

	UPROPERTY(EditAnywhere)
	float ScoreIncreaseSteps = 1.0f;

	UPROPERTY(EditAnywhere)
	float ScoreDeltaPerStep = 10.0f;

	UPROPERTY(EditAnywhere)
	float ScoreIncreaseTimeSteps = 0.3f;

	UPROPERTY(EditDefaultsOnly)
	bool bShouldDoStartGameAnimation = true;

	void UpdateScore();

	void OnGameWon();
	
	UFUNCTION()
	void OnPlayerDeath();

	void OnPauseGame();

	void OnUnPauseGame();

	UFUNCTION()
	void OnInstructionsRead();

	UFUNCTION()
	void OnStartGameAnimationFinished();

	

	FWidgetAnimationDynamicEvent StartGameAnimationEvent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> KillWidgetClass = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> StartWidgetClass = {nullptr};
private:
	float TargetScore = 0.0f;
	FTimerHandle ScoreIncreaseHandle;

	float TimeDelayInSeconds = 0.0f;

	float CurrentScore = 0.0f;
};
