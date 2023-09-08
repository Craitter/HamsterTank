// Fill out your copyright notice in the Description page of Project Settings.


#include "GameClasses/TanksterGameplayTags.h"

#include "GameplayTagsManager.h"
#include "ToolContextInterfaces.h"
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
	AddTag(AbilityInputTag_Fire, "InputTag.Ability.Fire", "LMB");
	// AddTag(Gameplay_NativeInputBlocked, "Gameplay.BlockInput.Native", "Block Native Input");
	// AddTag(Gameplay_AllInputBlocked, "Gameplay.BlockInput.All", "Block All Input");
	AddTag(Damage, "Meta.Damage", "Meta Tag for processing Damage");
	AddTag(Dead, "State.Dead", "Dead Actors get this Tag");

	AddTag(Hit_Front, "Hit.Direction.Front", "Simple Passing of HitDirection");
	AddTag(Hit_Back, "Hit.Direction.Back", "Simple Passing of HitDirection");
	AddTag(Hit_Left, "Hit.Direction.Left", "Simple Passing of HitDirection");
	AddTag(Hit_Right, "Hit.Direction.Right", "Simple Passing of HitDirection");
	AddTag(Hit_DirectionImmune, "Hit.Direction.Immune", "HitDirection doesnt matter to this");

	AddTag(Immortal, "State.Immortal", "Unit cant take damage");
	
	AddTag(Target_Predict_Location, "Target.Predict.Location", "Tag to Trigger Prediction of Movement for Location");
	AddTag(Target_Predict_All, "Target.Predict.All", "Tag to Trigger Prediction of Movement for Location and Rotation");
	AddTag(Cooldown_FireProjectile, "Ability.Cooldown.FireProjectile", "");
}
 
void FTanksterGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}