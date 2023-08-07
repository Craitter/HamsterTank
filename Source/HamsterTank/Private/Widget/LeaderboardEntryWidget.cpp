// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LeaderboardEntryWidget.h"

#include "LeaderboardSaveGame.h"
#include "Components/TextBlock.h"

void ULeaderboardEntryWidget::SetValues(const FLeaderboardEntry& Entry) const
{
	if(Rank && Name && Points)
	{
		FNumberFormattingOptions Options;
		Options.MaximumFractionalDigits = 0;
		Options.AlwaysSign = false;
		Rank->SetText(FText::AsNumber(Entry.Rank));
		Name->SetText(FText::FromString(Entry.Name));
		Points->SetText(FText::AsNumber(Entry.Score, &Options));
	}
}
