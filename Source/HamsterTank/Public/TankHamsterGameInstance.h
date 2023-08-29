// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundCue.h"
#include "TankHamsterGameInstance.generated.h"

struct FLeaderboardEntry;
class ULeaderboardSaveGame;
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

	virtual void Init() override;
	void StartGame();

	void OpenMainMenu();

	float GetMouseSensivity();
	void SetSliderValue(float NewValue, ESliderType Slider);
	float GetSliderValue(ESliderType Slider);
	void HandleSliderChanged(float NewValue, ESliderType Slider) const;
	void MouseSensitivityChanged(float NewValue) const;
	void MasterVolumeChanged(float NewValue) const;

	TWeakObjectPtr<ULeaderboardSaveGame> GetCurrentSaveGame() const;

	void PlayBackgroundMusic(TObjectPtr<USoundCue>) const;

	void ClearBackgroundMusic();

	void GetLeaderboardList(TArray<FLeaderboardEntry>& CurrentLeaderboard);

	void AddNameToLeaderboardList(FString PlayerName, float Points);
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

	UPROPERTY()
	TMap<ESliderType, float> SliderValues;

	TObjectPtr<ULeaderboardSaveGame> LeaderboardSaveGame = {nullptr};

	
};
