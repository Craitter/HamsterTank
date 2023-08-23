// Fill out your copyright notice in the Description page of Project Settings.


#include "TankHamsterGameInstance.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

class USoundCue;

UTankHamsterGameInstance::UTankHamsterGameInstance()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> BPSound(TEXT("/Game/SFX/HamsterTank_GamePlay_LOOP_Cue"));
	if(!ensure(BPSound.Object != nullptr)) return;
	GameMusic = BPSound.Object;
}

void UTankHamsterGameInstance::StartGame()
{
	check(GetWorld())
	BackgroundMusic = UGameplayStatics::CreateSound2D(this, GameMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	if(BackgroundMusic != nullptr)
	{
		BackgroundMusic->Play();
		BackgroundMusic->bStopWhenOwnerDestroyed = false;
	}
	GetWorld()->ServerTravel("/Game/Levels/DemoLevel");
}

void UTankHamsterGameInstance::OpenMainMenu()
{
	check(GetWorld())
	ClearBackgroundMusic();
	GetWorld()->ServerTravel("/Game/Levels/MainMenuMap");
}


void UTankHamsterGameInstance::SetSoundPercentage(float NewPercentage)
{
	// UGameplayStatics::SetSoundMixClassOverride(this, );
	// UGameplayStatics::PushSoundMixModifier()
	SoundPercentage = NewPercentage;
}

float UTankHamsterGameInstance::GetSoundPercentage() const
{
	return SoundPercentage;
}

bool UTankHamsterGameInstance::IsSoundMuted() const
{
	return bIsMuted;
}

void UTankHamsterGameInstance::PlayBackgroundMusic(TObjectPtr<USoundCue> InBackgroundMusic)
{
	if(BackgroundMusic != nullptr && BackgroundMusic->Sound == InBackgroundMusic)
	{
		return;
		
	}
	
	
}

void UTankHamsterGameInstance::ClearBackgroundMusic()
{
	if(BackgroundMusic != nullptr)
	{
		BackgroundMusic->Deactivate();
		BackgroundMusic->DestroyComponent();
		BackgroundMusic = nullptr;
	}
}
