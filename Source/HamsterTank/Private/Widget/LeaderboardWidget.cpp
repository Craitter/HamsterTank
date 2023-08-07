// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/LeaderboardWidget.h"

#include "LeaderboardSaveGame.h"
#include "Components/ScrollBox.h"
#include "Widget/LeaderboardEntryWidget.h"
#include "Widget/UISubsystem.h"

void ULeaderboardWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	Refresh();
}

void ULeaderboardWidget::Refresh()
{
	if(Leaderboard == nullptr || LeaderboardEntryClass == nullptr || !UISubsystem.IsValid())
	{
		return;
	}
	Leaderboard->ClearChildren();
	const TWeakObjectPtr<ULeaderboardEntryWidget> EmptyEntry = CreateWidget<ULeaderboardEntryWidget>(this, LeaderboardEntryClass);
	Leaderboard->AddChild(EmptyEntry.Get());
	TArray<FLeaderboardEntry> SaveGameLeaderboardEntries;
	UISubsystem->GetLeaderboardList(SaveGameLeaderboardEntries);
	int32 Rank = 1;
	for(int32 i = SaveGameLeaderboardEntries.Num() -1; i >= 0; i--)
	{
		
		SaveGameLeaderboardEntries[i].Rank = Rank;
		Rank++;
		TWeakObjectPtr<ULeaderboardEntryWidget> Entry = CreateWidget<ULeaderboardEntryWidget>(this, LeaderboardEntryClass);
		if(Entry.IsValid())
		{
			UE_LOG(LogTemp, Warning , TEXT("AddingEntry"));
			Entry->SetValues(SaveGameLeaderboardEntries[i]);
		}
		Leaderboard->AddChild(Entry.Get());
	}
}
