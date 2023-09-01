// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "TanksterAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTanksterAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:

	static UTanksterAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};
