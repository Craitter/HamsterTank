// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/VictoryWidget.h"

#include "Widget/CalculatePointsWidget.h"

bool UVictoryWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	if(VictoryAnimation)
	{
		Event.BindDynamic(this, &ThisClass::OnVictoryAnimationFinished);
		BindToAnimationFinished(VictoryAnimation, Event);
		
	}

	return true;
}

void UVictoryWidget::OnVictoryAnimationFinished()
{
	if(CalculatePointsWidget)
	{
		CalculatePointsWidget->Start();
	}
}
