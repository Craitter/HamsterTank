// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HamsterTankGameState.generated.h"

/**
 * 
 */
UCLASS()
class HAMSTERTANK_API AHamsterTankGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	bool HasMatchFinished() const;
	UFUNCTION()
	void FinishMatch();

	virtual void BeginPlay() override;
protected:
	bool bHasGameFinished = false;
};
