// Fill out your copyright notice in the Description page of Project Settings.



#include "GameClasses/HamsterTankGameState.h"


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
}
