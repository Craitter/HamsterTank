// Copyright Epic Games, Inc. All Rights Reserved.


#include "HamsterTankGameModeBase.h"


void AHamsterTankGameModeBase::PauseMatch() const
{
	HandleMatchPauseDelegate.Broadcast();
}

void AHamsterTankGameModeBase::UnpauseMatch() const
{
	HandleMatchUnPauseDelegate.Broadcast();
}

void AHamsterTankGameModeBase::RestartMatch()
{
	ResetLevel();
	HandleMatchRestartDelegate.Broadcast();
}

void AHamsterTankGameModeBase::EndMatch() const
{
	HandleMatchEndDelegate.Broadcast();
}

void AHamsterTankGameModeBase::MatchStart()
{
	check(GetWorld())
	InitialTimeDelay = GetWorld()->GetTimeSeconds();
	HandleMatchStartDelegate.Broadcast();
}

float AHamsterTankGameModeBase::GetInitialTimeDelay() const
{
	return InitialTimeDelay;
}
