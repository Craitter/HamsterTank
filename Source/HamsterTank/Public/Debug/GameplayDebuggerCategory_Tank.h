
#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"


enum class EDrivingState : uint8;
class APlayerController;
class AActor;


//resources:
//https://unrealcommunity.wiki/extending-gameplay-debugger-lcudh8ot
//https://forums.unrealengine.com/t/quick-c-tip-overriding-your-games-startupmodule-shutdownmodule-functions/118315
class FGameplayDebuggerCategory_Tank : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_Tank();
	virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;

	virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

	
protected:
	struct FRepData
	{
		// Put all data you want to display here
		FString ActorName;
		bool bIsPlayerControlled;
		bool bWasFallBackToPCPawn;

		//Movement
		FVector VelocityDirection;
		float CurrentSpeed;
		float CurrentMaxSpeed;
		EDrivingState DrivingState;
		float SlideDegree;
		bool bIsSliding;
		//Movement
		
        
		void Serialize(FArchive& Ar);
	};
    
	FRepData DataPack;
};
#endif
