// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HamsterTankGameModeBase.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandleMatchEndDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandleMatchRestartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandleMatchPauseDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandleMatchUnPauseDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandleMatchStartDelegate);

UCLASS()
class HAMSTERTANK_API AHamsterTankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void PauseMatch() const;
	void UnpauseMatch() const;
	void RestartMatch();
	void EndMatch() const;

	void MatchStart();
	float GetInitialTimeDelay() const;

	FHandleMatchStartDelegate HandleMatchStartDelegate;
	FHandleMatchEndDelegate HandleMatchEndDelegate;
	FHandleMatchRestartDelegate HandleMatchRestartDelegate;
	FHandleMatchPauseDelegate HandleMatchPauseDelegate;
	FHandleMatchUnPauseDelegate HandleMatchUnPauseDelegate;

private:
	float InitialTimeDelay = 0.0f;
};
