// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BulletIconWidget.h"

#include "Components/Image.h"


bool UBulletIconWidget::Initialize()
{
	bIsFilled = false;
	return Super::Initialize();
}

void UBulletIconWidget::SetIsFilledAmmo(bool bNewState)
{
	if(CurrentBulletImage != nullptr)
	{
		bIsFilled = bNewState;

		if(bIsFilled)
		{
			CurrentBulletImage->SetBrushFromTexture(FilledAmmo);
		}
		else
		{
			CurrentBulletImage->SetBrushFromTexture(UsedAmmo);
		}
	}
}
