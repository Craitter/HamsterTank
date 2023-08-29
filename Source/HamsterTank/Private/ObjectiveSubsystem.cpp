// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveSubsystem.h"

#include "Components/CherryObjectiveComponent.h"
#include "Components/FireProjectileComponent.h"
#include "Components/HealthComponent.h"
#include "HamsterTank/HamsterTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"



void FObjectiveScore::AddScoreDelta(const float DeltaScore)
{
	Score += DeltaScore;
	OnScoreChangedDelegateHandle.Broadcast(Score);
}

void FObjectiveScore::AddCherryDelta(int32 DeltaCherries)
{
	Cherries += DeltaCherries;
	if(Cherries < 0)
	{
		Cherries = 0;
	}
	OnCherryCollectedDelegateHandle.Broadcast(Cherries);
}

void UObjectiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UObjectiveSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	const TWeakObjectPtr<AHamsterTankGameModeBase> HamsterTankGameModeBase =
		Cast<AHamsterTankGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(HamsterTankGameModeBase.IsValid())
	{
		HamsterTankGameModeBase->HandleMatchEndDelegate.AddDynamic(this, &ThisClass::GatherScore);
	}
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

void UObjectiveSubsystem::CherryCollected(const TWeakObjectPtr<AController> Player, const int32 Delta)
{
	FObjectiveScore* Score = Players.Find(Player);
	if(Score == nullptr)
	{
		return;
	}
	Score->AddCherryDelta(Delta);
}

void UObjectiveSubsystem::TowerDestroyed(const TWeakObjectPtr<AController> Player)
{
	FObjectiveScore* Score = Players.Find(Player);
	if(Score == nullptr)
	{
		return;
	}
	Score->AddScoreDelta(50.0f);
	OnTowerDestroyed.Broadcast();
}

void UObjectiveSubsystem::GatherScore()
{
	for (auto& Player : Players)
	{
		if(Player.Key != nullptr)
		{
			const TWeakObjectPtr<APawn> ControlledPawn = Player.Key->GetPawn();
			GetScoreForPawn(ControlledPawn, &Player.Value);
		}
	}
}

void UObjectiveSubsystem::GetScoreForPawn(TWeakObjectPtr<APawn> Pawn, FObjectiveScore* Score)
{
	if(!Pawn.IsValid() || Score == nullptr)
	{
		return;
	}
	const TWeakObjectPtr<UHealthComponent> HealthComponent = Pawn->FindComponentByClass<UHealthComponent>();
	const TWeakObjectPtr<UFireProjectileComponent> FireProjectileComponent = Pawn->FindComponentByClass<UFireProjectileComponent>();
	const TWeakObjectPtr<UCherryObjectiveComponent> CherryObjectiveComponent = Pawn->FindComponentByClass<UCherryObjectiveComponent>();
	
	if(!HealthComponent.IsValid() || !FireProjectileComponent.IsValid() || !CherryObjectiveComponent.IsValid())
	{
		return;
	}
	
	Score->SetHealth(HealthComponent->GetCurrentHealth());
	Score->SetCherries(CherryObjectiveComponent->GetCurrentCherries());
	Score->SetAmmo(FireProjectileComponent->GetCurrentAmmo());
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
