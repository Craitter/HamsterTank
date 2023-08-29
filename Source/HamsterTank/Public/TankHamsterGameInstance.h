// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundCue.h"
#include "TankHamsterGameInstance.generated.h"

enum class ESliderType : uint8;
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

	
	void SetSliderValue(float NewValue, ESliderType Slider);
	float GetSliderValue(ESliderType Slider);
	void HandleSliderChanged(float NewValue, ESliderType Slider) const;
	void MouseSensitivityChanged(float NewValue) const;
	void MasterVolumeChanged(float NewValue) const;

	bool IsSoundMuted() const;

	void PlayBackgroundMusic(TObjectPtr<USoundCue>);

	void ClearBackgroundMusic();
private:
	bool bIsMuted = false;

	float SoundPercentage = 1.0f;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BackgroundMusic = {nullptr};
	UPROPERTY()
	TObjectPtr<USoundCue> GameMusic = {nullptr};
	UPROPERTY()
	TObjectPtr<USoundMix> DefaultMix = {nullptr};
	UPROPERTY()
	TObjectPtr<USoundClass> DefaultSoundClass = {nullptr};

	TMap<ESliderType, float> SliderValues;
};
