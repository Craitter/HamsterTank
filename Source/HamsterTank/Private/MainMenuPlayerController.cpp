// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"


#include "Blueprint/UserWidget.h"
#include "Widget/UISubsystem.h"


void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const TWeakObjectPtr<UGameInstance> GameInstance = GetGameInstance();
	if(GameInstance.IsValid())
	{
		const TWeakObjectPtr<UUISubsystem> UISubsystem = GameInstance->GetSubsystem<UUISubsystem>();
		if(UISubsystem.IsValid())
		{
			const TWeakObjectPtr<UUserWidget> MainMenu = UISubsystem->OpenWidget(MainMenuWidget);
			SetShowMouseCursor(true);
			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(MainMenu->TakeWidget());
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(Mode);
		}
	}
}
