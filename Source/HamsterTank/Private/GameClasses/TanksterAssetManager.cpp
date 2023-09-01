// Fill out your copyright notice in the Description page of Project Settings.



#include "GameClasses/TanksterAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "GameClasses/TanksterGameplayTags.h"


UTanksterAssetManager& UTanksterAssetManager::Get()
{
	UTanksterAssetManager* Singleton = Cast<UTanksterAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GDAssetManager!"));
		return *NewObject<UTanksterAssetManager>();	 // never calls this
	}
}

void UTanksterAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();

	// UGameplayCueManager::Get()

	//Load Native Tags
	FTanksterGameplayTags::InitializeNativeTags();
}
