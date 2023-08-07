// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "CalculatePointsWidget.generated.h"

struct FObjectiveScore;
class ULeaderboardWidget;
class UWidgetSwitcher;
class UButton;
class UOverlayButtonsWidget;
class UEditableTextBox;
class UTextBlock;
/**
 * 
 */

UENUM()
enum class ECalculationOrder : uint8
{
	None,
	
	Base,

	Cherries,

	Bullet,

	Health,
	
	Time,

	Total,

	Max,
};

UCLASS()
class HAMSTERTANK_API UCalculatePointsWidget : public UTankBaseWidget
{
	GENERATED_BODY()

public:
	void Start();

	virtual void NativePreConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULeaderboardWidget> LeaderboardWidget = {nullptr};
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreBase = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountCherries = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Score_Cherries = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountBullet = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreBullet = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountHealth = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreHealth = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeAmount = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreTime = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalScore = {nullptr};
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> Name = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Confirm = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlayButtonsWidget> Buttons = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	FRuntimeFloatCurve PointTimeCurve;

	bool ShouldSkipUpdate() const;

private:
	void CalculateNumbers(UTextBlock* TextBlockToModify, float StepSize, float EndSize);

	UFUNCTION()
	void OnTextCommittedCallback(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnNameConfirmed();

	void CalculateNext();

	float TotalPoints = 0.0f;

	FObjectiveScore* Score = nullptr;

	ECalculationOrder Order = ECalculationOrder::None;

	float CurrentStepSize = 0.0f;
	float CurrentValue = 0.0f;
	float CurrentTopEnd = 0.0f;

	int32 MaxStepsPerTick = 5;
	UPROPERTY()
	UTextBlock* CurrentTextBlock = {nullptr};

	void SetCalculateBase();
	void SetCalculateCherries();
	void SetCalculateBullet();
	void SetCalculateHealth();
	void SetCalculateTime();
	void SetCalculateTotal();
};
