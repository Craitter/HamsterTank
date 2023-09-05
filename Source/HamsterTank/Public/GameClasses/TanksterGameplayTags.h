// Fill out your copyright notice in the Description page of Project Settings.

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * MyGameplayTags
 *
 *	Singleton containing native gameplay tags.
 */
struct FTanksterGameplayTags
{
public:

	static const FTanksterGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	//Input Tags
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Look_Stick;
	FGameplayTag AbilityInputTag_Fire;

	// FGameplayTag Gameplay_AbilityInputBlocked;
	// FGameplayTag Gameplay_NativeInputBlocked;
	// FGameplayTag Gameplay_AllInputBlocked;

protected:

	//Registers all of the tags with the GameplayTags Manager
	void AddAllTags(UGameplayTagsManager& Manager);

	//Helper function used by AddAllTags to register a single tag with the GameplayTags Manager
	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	static FTanksterGameplayTags GameplayTags;
};