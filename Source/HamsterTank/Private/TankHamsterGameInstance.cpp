// Fill out your copyright notice in the Description page of Project Settings.


#include "TankHamsterGameInstance.h"

#include "TankPlayerController.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Widget/MenuWidgets/GenericSliderWidget.h"

class USoundCue;

UTankHamsterGameInstance::UTankHamsterGameInstance()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> BPSound(TEXT("/Game/SFX/HamsterTank_GamePlay_LOOP_Cue"));
	if(!ensure(BPSound.Object != nullptr)) return;
	GameMusic = BPSound.Object;

	static ConstructorHelpers::FObjectFinder<USoundMix> BPSoundMix(TEXT("/Game/SFX/SCM_MasterVolume"));
	if(!ensure(BPSoundMix.Object != nullptr)) return;
	DefaultMix = BPSoundMix.Object;

	static ConstructorHelpers::FObjectFinder<USoundClass> BPSoundClass(TEXT("/Engine/EngineSounds/Master"));
	if(!ensure(BPSoundClass.Object != nullptr)) return;
	DefaultSoundClass = BPSoundClass.Object;
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

void UTankHamsterGameInstance::SetSliderValue(const float NewValue, const ESliderType Slider)
{
	float& Value = SliderValues.FindOrAdd(Slider);
	Value = NewValue;
	HandleSliderChanged(NewValue, Slider);
}

float UTankHamsterGameInstance::GetSliderValue(const ESliderType Slider) 
{
	const float* Value = SliderValues.Find(Slider);
	if(Value != nullptr)
	{
		return *Value;
	}
	else
	{
		return -1.0f;
	}
}

void UTankHamsterGameInstance::HandleSliderChanged(const float NewValue, const ESliderType Slider) const
{
	switch (Slider)
	{
	case ESliderType::None:
		break;
	case ESliderType::MasterVolume:
		MasterVolumeChanged(NewValue);
		break;
	case ESliderType::MouseSensitivity:
		MouseSensitivityChanged(NewValue);
		break;
	default: ;
	}
}

void UTankHamsterGameInstance::MouseSensitivityChanged(const float NewValue) const
{
	const TWeakObjectPtr<ATankPlayerController> TankPlayerController = Cast<ATankPlayerController>(GetFirstLocalPlayerController());
	if(TankPlayerController.IsValid())
	{
		TankPlayerController->SetMouseSensitivity(NewValue);
	}
}

void UTankHamsterGameInstance::MasterVolumeChanged(const float NewValue) const
{
	UGameplayStatics::SetSoundMixClassOverride(this, DefaultMix, DefaultSoundClass, NewValue, 1.0f, 0.0f);
	UGameplayStatics::PushSoundMixModifier(this, DefaultMix);
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
