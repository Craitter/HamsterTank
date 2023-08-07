// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LeaderboardSaveGame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 Rank = 1;
	UPROPERTY()
	FString Name = FString();
	UPROPERTY()
	float Score = 0.0f;
	
	bool operator < (const FLeaderboardEntry Entry) const {return Score < Entry.Score;}
};

UCLASS()
class HAMSTERTANK_API ULeaderboardSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	ULeaderboardSaveGame();
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	TArray<FLeaderboardEntry> Leaderboard;
};
