// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/MainWidgets/InstructionsWidget.h"

#include "Components/Button.h"
#include "HamsterTank/HamsterTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"


bool UInstructionsWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	HamsterTankGameModeBase = Cast<AHamsterTankGameModeBase>(UGameplayStatics::GetGameMode(this));
	

	if(!ensure(InstructionsButton != nullptr)) return false;
	
	InstructionsButton->OnClicked.AddDynamic(this, &ThisClass::OnInstructionsRead);
	PlayAnimation(ContinueAnimation, 0, 200);

	StartGameAnimationEvent.BindDynamic(this, &ThisClass::OnStartGameAnimationFinished);
	BindToAnimationFinished(StartGameAnimation, StartGameAnimationEvent);
	
	
	return true;
}

void UInstructionsWidget::OnStartGameAnimationFinished()
{
	if(HamsterTankGameModeBase.IsValid())
	{
		HamsterTankGameModeBase->MatchStart();
	}
}

void UInstructionsWidget::OnInstructionsRead()
{
	if(InstructionsButton != nullptr)
	{
		StopAnimation(ContinueAnimation);
		InstructionsButton->OnClicked.Clear();
		InstructionsButton->SetVisibility(ESlateVisibility::Hidden);
		if(GetOwningPlayer())
		{
			GetOwningPlayer()->SetShowMouseCursor(false);
			const FInputModeGameOnly Game;
			
			GetOwningPlayer()->SetInputMode(Game);
		}
	}
	PlayAnimation(StartGameAnimation);
}
