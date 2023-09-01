// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Sound/SoundCue.h"
#include "TankHamsterGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTankHamsterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTankHamsterGameInstance();

	virtual void Init() override;

private:	
};
