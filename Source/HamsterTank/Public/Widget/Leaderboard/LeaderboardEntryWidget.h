// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LeaderboardWidget.h"
#include "Widget/TankBaseWidget.h"
#include "LeaderboardEntryWidget.generated.h"

/**
 * 
 */

struct FLeaderboardEntry;
class UTextBlock;

UCLASS()
class HAMSTERTANK_API ULeaderboardEntryWidget : public UTankBaseWidget
{
	GENERATED_BODY()

public:
	void SetValues(const FLeaderboardEntry& Entry) const;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Rank = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Name = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Points = {nullptr};
};
