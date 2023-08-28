// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TankBaseWidget.h"

#include "TankHamsterGameInstance.h"
#include "Components/MenuAnchor.h"
#include "Widget/UISubsystem.h"

void UTankBaseWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	GameInstance = Cast<UTankHamsterGameInstance>(GetGameInstance());
	if(GameInstance.IsValid())
	{
		UISubsystem = GameInstance->GetSubsystem<UUISubsystem>();
	}
}
