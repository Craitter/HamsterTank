// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"


#include "Blueprint/UserWidget.h"


void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	const TWeakObjectPtr<UUserWidget> MainMenu = CreateWidget<UUserWidget>(this, MainMenuWidget);
	if(MainMenu.IsValid())
	{
		MainMenu->AddToViewport();
		SetShowMouseCursor(true);
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(MainMenu->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(Mode);
	}
	
}
