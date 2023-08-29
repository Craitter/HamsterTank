// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDPointsCountWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHUDPointsCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	void OnScoreChanged(float NewScore);

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InGameScore = {nullptr};
	
	UPROPERTY(EditAnywhere)
	float ScoreIncreaseSteps = 1.0f;

	UPROPERTY(EditAnywhere)
	float ScoreDeltaPerStep = 50.0f;

	UPROPERTY(EditAnywhere)
	float ScoreIncreaseTimeSteps = 0.01f;
private:
	float TargetScore = 0.0f;
	FTimerHandle ScoreIncreaseHandle;

	float CurrentScore = 0.0f;
	
	void UpdateScore();
};
