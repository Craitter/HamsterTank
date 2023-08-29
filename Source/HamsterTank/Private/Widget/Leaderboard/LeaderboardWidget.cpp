// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/Leaderboard/LeaderboardWidget.h"

#include "LeaderboardSaveGame.h"
#include "TankHamsterGameInstance.h"
#include "Components/ScrollBox.h"
#include "Widget/Leaderboard/LeaderboardEntryWidget.h"

void ULeaderboardWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Refresh();
}

void ULeaderboardWidget::Refresh()
{
	if(Leaderboard == nullptr || LeaderboardEntryClass == nullptr || !GameInstance.IsValid())
	{
		return;
	}
	Leaderboard->ClearChildren();
	TArray<FLeaderboardEntry> SaveGameLeaderboardEntries;
	GameInstance->GetLeaderboardList(SaveGameLeaderboardEntries);
	int32 Rank = 1;
	for(int32 i = SaveGameLeaderboardEntries.Num() -1; i >= 0; i--)
	{
		
		SaveGameLeaderboardEntries[i].Rank = Rank;
		Rank++;
		TWeakObjectPtr<ULeaderboardEntryWidget> Entry = CreateWidget<ULeaderboardEntryWidget>(this, LeaderboardEntryClass);
		if(Entry.IsValid())
		{
			Entry->SetValues(SaveGameLeaderboardEntries[i]);
		}
		Leaderboard->AddChild(Entry.Get());
	}
}
