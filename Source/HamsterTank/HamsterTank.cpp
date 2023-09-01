// Copyright Epic Games, Inc. All Rights Reserved.


#include "HamsterTank.h"

#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FHamsterTankModule, HamsterTank, "HamsterTank");

void FHamsterTankModule::StartupModule()
{
	IModuleInterface::StartupModule();

#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	GameplayDebuggerModule.RegisterCategory("Tank", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Tank::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
#endif // WITH_GAMEPLAY_DEBUGGER
}

void FHamsterTankModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if (IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("Tank");
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
#endif // WITH_GAMEPLAY_DEBUGGER
	
	IModuleInterface::ShutdownModule();
}

bool FHamsterTankModule::IsGameModule() const
{
	return true;
}
