// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUDWidgets/HUDMatchTimeWidget.h"

#include "Components/TextBlock.h"
#include "HamsterTank/HamsterTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UHUDMatchTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	check(GetWorld())
	
	if(PlayTime != nullptr)
	{
		
		const FTimespan Time = FTimespan::FromSeconds(GetWorld()->GetTimeSeconds() - InitialTimeDelay);
		
		const FText Minutes = FText::AsNumber(Time.GetMinutes(), &NumberFormattingOptions);
		const FText Seconds = FText::AsNumber(Time.GetSeconds(), &NumberFormattingOptions);
		const FText TimeText = FText::FromString("{m}:{s}");
		
		FFormatNamedArguments Args;
		Args.Add("m", Minutes);
		Args.Add("s", Seconds);
		PlayTime->SetText(FText::Format(TimeText, Args));
	}
}

bool UHUDMatchTimeWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	const TWeakObjectPtr<AHamsterTankGameModeBase> HamsterTankGameModeBase = Cast<AHamsterTankGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(HamsterTankGameModeBase.IsValid())
	{
		InitialTimeDelay = HamsterTankGameModeBase->GetInitialTimeDelay();
	}
	NumberFormattingOptions.AlwaysSign = false;
	NumberFormattingOptions.UseGrouping = true;
	NumberFormattingOptions.MinimumIntegralDigits = 2;
	NumberFormattingOptions.MaximumIntegralDigits = 2;

	return true;
}
