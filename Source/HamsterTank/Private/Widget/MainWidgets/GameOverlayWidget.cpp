// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/MainWidgets/GameOverlayWidget.h"

#include "ObjectiveSubsystem.h"


bool UGameOverlayWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(GetWorld())
	{
		const TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
		if(ObjectiveSubsystem.IsValid())
		{
			ObjectiveSubsystem->OnTowerDestroyed.AddUObject(this, &ThisClass::DisplayKill);
		}
	}
	
	return true;
}

void UGameOverlayWidget::DisplayKill()
{
	if(DisplayKillAnimation != nullptr)
	{
		PlayAnimationForward(DisplayKillAnimation);
	}
}
