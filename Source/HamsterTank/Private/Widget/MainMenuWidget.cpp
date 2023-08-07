// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainMenuWidget.h"

#include "TankHamsterGameInstance.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

bool UMainMenuWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	if(!ensure(PlayButton != nullptr)) return false;
	if(!ensure(LeaderboardButton != nullptr)) return false;
	if(!ensure(DesktopButton != nullptr)) return false;
	if(!ensure(SoundSlider != nullptr)) return false;
	if(!ensure(BackButton != nullptr)) return false;
	if(!ensure(Switcher != nullptr)) return false;
	if(!ensure(LeaderboardScrollBox != nullptr)) return false;

	PlayButton->OnPressed.AddDynamic(this, &ThisClass::OnPlayButtonPressed);
	LeaderboardButton->OnPressed.AddDynamic(this, &ThisClass::OnLeaderboardButtonPressed);
	DesktopButton->OnPressed.AddDynamic(this, &ThisClass::OnDesktopButtonPressed);
	BackButton->OnPressed.AddDynamic(this, &ThisClass::OnBackButtonPressed);
	
	return true;
}

void UMainMenuWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UMainMenuWidget::OnPlayButtonPressed()
{
	if(GameInstance.IsValid())
	{
		GameInstance->StartGame();
	}
}

void UMainMenuWidget::OnLeaderboardButtonPressed()
{
	PlayAnimationForward(SwapToAchievements);
}

void UMainMenuWidget::OnDesktopButtonPressed()
{
	const TWeakObjectPtr<APlayerController> PlayerController = GetOwningPlayer();
	if(PlayerController.IsValid())
	{
		PlayerController->ConsoleCommand("quit");
	}
}

void UMainMenuWidget::OnBackButtonPressed()
{
	PlayAnimationReverse(SwapToAchievements);
}

