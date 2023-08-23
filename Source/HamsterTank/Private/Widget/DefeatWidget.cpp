// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DefeatWidget.h"

#include "Widget/OverlayButtonsWidget.h"

void UDefeatWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	if(Buttons != nullptr)
	{
		Buttons->ShowResumeButton(false);
	}
}

void UDefeatWidget::PlayDefeatAnimation()
{
	PlayAnimationForward(LoseAnimation);
}
