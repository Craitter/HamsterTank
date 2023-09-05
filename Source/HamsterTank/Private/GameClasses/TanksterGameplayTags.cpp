// Fill out your copyright notice in the Description page of Project Settings.


#include "GameClasses/TanksterGameplayTags.h"

#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"
 
FTanksterGameplayTags FTanksterGameplayTags::GameplayTags;
 
void FTanksterGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
 
	GameplayTags.AddAllTags(GameplayTagsManager);
 
	GameplayTagsManager.DoneAddingNativeTags();
}
 
void FTanksterGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	AddTag(InputTag_Look_Stick, "InputTag.Look.Stick", "Look (stick) input.");
	// AddTag(Gameplay_AbilityInputBlocked, "Gameplay.BlockInput.Ability", "Block Ability Input");
	// AddTag(Gameplay_NativeInputBlocked, "Gameplay.BlockInput.Native", "Block Native Input");
	// AddTag(Gameplay_AllInputBlocked, "Gameplay.BlockInput.All", "Block All Input");
}
 
void FTanksterGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}