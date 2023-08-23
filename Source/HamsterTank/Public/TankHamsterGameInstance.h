// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundCue.h"
#include "TankHamsterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTankHamsterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTankHamsterGameInstance();
	
	void StartGame();

	void OpenMainMenu();

	void SetSoundPercentage(float NewPercentage);

	float GetSoundPercentage() const;

	bool IsSoundMuted() const;

	void PlayBackgroundMusic(TObjectPtr<USoundCue>);

	void ClearBackgroundMusic();
private:
	bool bIsMuted = false;

	float SoundPercentage = 1.0f;

	TObjectPtr<USoundMix> MixerForVolume = {nullptr};

	UPROPERTY()
	TObjectPtr<UAudioComponent> BackgroundMusic = {nullptr};
	UPROPERTY()
	TObjectPtr<USoundCue> GameMusic = {nullptr};
};
