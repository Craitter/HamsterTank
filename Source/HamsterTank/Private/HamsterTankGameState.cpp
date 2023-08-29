// Fill out your copyright notice in the Description page of Project Settings.


#include "HamsterTankGameState.h"

#include "HamsterTank/HamsterTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"

bool AHamsterTankGameState::HasMatchFinished() const
{
	return bHasGameFinished;
}

void AHamsterTankGameState::FinishMatch()
{
	bHasGameFinished = true;
}

void AHamsterTankGameState::BeginPlay()
{
	Super::BeginPlay();

	const TWeakObjectPtr<AHamsterTankGameModeBase> HamsterTankGameModeBase = Cast<AHamsterTankGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(HamsterTankGameModeBase.IsValid())
	{
		HamsterTankGameModeBase->HandleMatchEndDelegate.AddDynamic(this, &ThisClass::FinishMatch);
	}
}
