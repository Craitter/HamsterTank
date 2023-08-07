// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TankHamsterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTankHamsterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void StartGame() const;

	void OpenMainMenu() const;

	void SetSoundMuted(bool bNewValue);

	void SetSoundPercentage(float NewPercentage);

	float GetSoundPercentage() const;

	bool IsSoundMuted() const;
private:
	bool bIsMuted = false;

	float SoundPercentage = 1.0f;
};
