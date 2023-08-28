// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OverlayButtonsWidget.h"

#include "Components/Button.h"
#include "Widget/UISubsystem.h"

bool UOverlayButtonsWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(!ensure(ResumeButton != nullptr)) return false;
	if(!ensure(RestartButton != nullptr)) return false;
	if(!ensure(HomeButton != nullptr)) return false;

	ResumeButton->OnPressed.AddDynamic(this, &ThisClass::OnResumePressed);
	RestartButton->OnPressed.AddDynamic(this, &ThisClass::OnRestartPressed);
	HomeButton->OnPressed.AddDynamic(this, &ThisClass::OnHomePressed);	
	
	
	return true;
}

void UOverlayButtonsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UOverlayButtonsWidget::ShowResumeButton(bool bShowResume) const
{
	if(ResumeButton == nullptr)
	{
		return;
	}
	
	if(bShowResume)
	{
		ResumeButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ResumeButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UOverlayButtonsWidget::OnResumePressed()
{
	if(UISubsystem.IsValid())
	{
		UISubsystem->UnPauseGame();
	}
}

void UOverlayButtonsWidget::OnRestartPressed()
{
	if(UISubsystem.IsValid())
	{
		UISubsystem->RestartLevel();
	}
}

void UOverlayButtonsWidget::OnHomePressed()
{
	if(UISubsystem.IsValid())
	{
		UISubsystem->BackToMainMenu();
	}
}