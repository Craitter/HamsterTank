// Fill out your copyright notice in the Description page of Project Settings.


#include "TankHamsterGameInstance.h"

void UTankHamsterGameInstance::StartGame() const
{
	check(GetWorld())
	GetWorld()->ServerTravel("/Game/Levels/MovementUnitTest");
}

void UTankHamsterGameInstance::OpenMainMenu() const
{
	check(GetWorld())
	GetWorld()->ServerTravel("/Game/Levels/MainMenuMap");
}

void UTankHamsterGameInstance::SetSoundMuted(bool bNewValue)
{
	//Todo: Mute Game
}

void UTankHamsterGameInstance::SetSoundPercentage(float NewPercentage)
{
	//Todo: SetSoundPercentage
}

float UTankHamsterGameInstance::GetSoundPercentage() const
{
	return SoundPercentage;
}

bool UTankHamsterGameInstance::IsSoundMuted() const
{
	return bIsMuted;
}
