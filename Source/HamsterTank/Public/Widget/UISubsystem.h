// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UISubsystem.generated.h"


class ULeaderboardSaveGame;
struct FLeaderboardEntry;
class UTankBaseWidget;
class UGameOverlayWidget;
class UTankHamsterGameInstance;
/**
 * 
 */


DECLARE_MULTICAST_DELEGATE(FOnGamePause)
DECLARE_MULTICAST_DELEGATE(FOnGameUnPause)
DECLARE_MULTICAST_DELEGATE(FOnRestartLevel)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_MULTICAST_DELEGATE(FOnGameWon)
DECLARE_MULTICAST_DELEGATE(FOnPointsCalculated)

UCLASS()
class HAMSTERTANK_API UUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FOnGamePause PauseGameDelegate;
	TWeakObjectPtr<UUserWidget> PauseGame() const;
	FOnGameUnPause UnPauseGameDelegate;
	void UnPauseGame() const;

	
	FOnRestartLevel RestartLevelDelegate;
	void RestartLevel();

	void BackToMainMenu() const;

	float GetGameTimeDelay();
	void AddTimeDelay(float DeltaDelay);
	void ResetTimeDelay();
	float TotalTimeDelay = 0.0f;

	void GetLeaderboardList(TArray<FLeaderboardEntry>& CurrentLeaderboard);

	void AddNameToLeaderboardList(FString Name, float Points);

	TWeakObjectPtr<UUserWidget> OpenWidget(TSubclassOf<UTankBaseWidget> WidgetToOpen);
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;
	FOnGameWon OnGameWon;
	FOnPointsCalculated OnPointsCalculated;
	
	void NotifyPlayerDead();
	void NotifyGameWon();
protected:
	TWeakObjectPtr<UTankHamsterGameInstance> GameInstance = {nullptr};
	TWeakObjectPtr<UTankBaseWidget> ActiveWidget = {nullptr};
	TObjectPtr<ULeaderboardSaveGame> LeaderboardSaveGame = {nullptr};
};
