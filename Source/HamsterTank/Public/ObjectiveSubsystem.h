// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectiveSubsystem.generated.h"

/**
 * 
 */
struct FObjectiveScore;


DECLARE_MULTICAST_DELEGATE(FOnTowerDestroyed)
DECLARE_DELEGATE_OneParam(FOnGameEndDelegate, FObjectiveScore&)

USTRUCT(BlueprintType)
struct FObjectiveScore
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	float Steps = 200.0f;
	
	void AddScoreDelta(float DeltaScore);

	void AddCherryDelta(int32 DeltaCherries);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreChangedDelegate, float)
	FOnScoreChangedDelegate OnScoreChangedDelegateHandle;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnCherryCollectedDelegate, int32)
	FOnCherryCollectedDelegate OnCherryCollectedDelegateHandle;
	
	void SetOwningPlayer(const TWeakObjectPtr<AController> InOwningPlayer)
	{
		OwningPlayer = InOwningPlayer;
	}
	float GetCurrentScore() const {return Score;}

	void SetCherries(const int32 CherryAmount)
	{
		Cherries = CherryAmount;
	}
	void SetHealth(const float HealthAmount)
	{
		Health = HealthAmount;
	}
	void SetAmmo(const float AmmoAmount)
	{
		Ammo = AmmoAmount;
	}
	void SetTime(const FTimespan TimeTaken)
	{
		Time = TimeTaken;
	}

	float GetScore() const
	{
		return Score;
	}

	int32 GetCherries() const
	{
		return Cherries;
	}
	float GetHealth() const
	{
		return Health;
	}
	float GetAmmo() const
	{
		return Ammo;
	}
	FTimespan GetTime() const
	{
		return Time;
	}

	float GetCherryScore() const
	{
		return Cherries * 1000;
	}
	float GetHealthScore() const
	{
		return Health * 500;
	}
	float GetAmmoScore() const
	{
		return Ammo * 100;
	}
	float GetTimeScore() const
	{
		// if(Curve != nullptr)
		// {
		// 	return Curve->GetFloatValue(Time.GetTotalSeconds());
		// }
		return 0.0f;
	}

protected:
	UPROPERTY(EditDefaultsOnly)
	float CherryMultiplier = 1000.0f;

	UPROPERTY(EditDefaultsOnly)
	float HealthMultiplier = 500.0f;

	UPROPERTY(EditDefaultsOnly)
	float AmmoMultiplier = 100.0f;

	
private:
	TWeakObjectPtr<AController> OwningPlayer = {nullptr};
	float Score = 0.0f;

	int32 Cherries = 0.0f;

	float Health = 0.0f;

	float Ammo = 0.0f;

	FTimespan Time = 0.0f;
};



UCLASS()
class HAMSTERTANK_API UObjectiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RegisterPlayer(TWeakObjectPtr<AController> Player);
	void UnregisterPlayer(TWeakObjectPtr<AController> Player);

	void CherryCollected(TWeakObjectPtr<AController> Player, int32 Delta);
	
	void TowerDestroyed(TWeakObjectPtr<AController> Player);

	void EndGame(TWeakObjectPtr<AController> Player);

	TMulticastDelegate<void(float)>* GetOnScoreChangedDelegate(TWeakObjectPtr<APlayerController> Player);

	FObjectiveScore* GetScore(TWeakObjectPtr<AController> Player);

	FOnTowerDestroyed OnTowerDestroyed;

	FOnGameEndDelegate OnGameEndDelegate;

protected:
	UPROPERTY()
	TMap<TWeakObjectPtr<AController>, FObjectiveScore> Players;
};
