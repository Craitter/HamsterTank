// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveSubsystem.h"
#include "Widget/UISubsystem.h"

#include "TankPlayerController.h"



void FObjectiveScore::AddScoreDelta(const float DeltaScore)
{
	Score += DeltaScore;
	OnScoreChangedDelegateHandle.Broadcast(Score);
}

void UObjectiveSubsystem::RegisterPlayer(TWeakObjectPtr<AController> Player)
{
	FObjectiveScore& Score = Players.FindOrAdd(Player);
	Score.SetOwningPlayer(Player);
}

void UObjectiveSubsystem::UnregisterPlayer(TWeakObjectPtr<AController> Player)
{
	Players.Remove(Player);
}

void UObjectiveSubsystem::TowerDestroyed(TWeakObjectPtr<AController> Player)
{
	FObjectiveScore* Score = Players.Find(Player);
	if(Score == nullptr)
	{
		return;
	}
	Score->AddScoreDelta(50.0f);
	OnTowerDestroyed.Broadcast();
}

void UObjectiveSubsystem::EndGame(const TWeakObjectPtr<AController> Player)
{
	FObjectiveScore* Score = Players.Find(Player);

	if(Score != nullptr)
	{
		if(OnGameEndDelegate.ExecuteIfBound(*Score))
		{
			const TWeakObjectPtr<UGameInstance> GameInstance = GetGameInstance();
			if(GameInstance.IsValid())
			{
				const TWeakObjectPtr<UUISubsystem> UISubsystem = GameInstance->GetSubsystem<UUISubsystem>();
				if(UISubsystem.IsValid())
				{
					UISubsystem->OnGameWon.Broadcast();
					return;
				}
			}
		}
	}
}

TMulticastDelegate<void(float)>* UObjectiveSubsystem::GetOnScoreChangedDelegate(
	const TWeakObjectPtr<APlayerController> Player)
{
	FObjectiveScore* Score = Players.Find(Player);
	if(Score == nullptr)
	{
		return nullptr;
	}
	return &Score->OnScoreChangedDelegateHandle;
}

FObjectiveScore* UObjectiveSubsystem::GetScore(const TWeakObjectPtr<AController> Player)
{
	return Players.Find(Player);
}
