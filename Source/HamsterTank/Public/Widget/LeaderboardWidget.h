// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBaseWidget.h"
#include "LeaderboardWidget.generated.h"

/**
 * 
 */


class ULeaderboardEntryWidget;
class UScrollBox;



UCLASS()
class HAMSTERTANK_API ULeaderboardWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;

	void Refresh();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> Leaderboard = {nullptr};

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULeaderboardEntryWidget> LeaderboardEntryClass = {nullptr};
	
};
