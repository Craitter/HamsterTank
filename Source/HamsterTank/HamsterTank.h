// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define DEFAULT_SAVE_SLOT							TEXT("DefaultSlot")
#define DEFAULT_PAWN_SCALE							0.1428571428571429f

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "Public\Debug\GameplayDebuggerCategory_Tank.h"
#endif // WITH_GAMEPLAY_DEBUGGER
class FHamsterTankModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	/**
	 * Returns true if this module hosts gameplay code
	 *
	 * @return True for "gameplay modules", or false for engine code modules, plug-ins, etc.
	 */
	virtual bool IsGameModule() const override;
};
