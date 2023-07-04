

#include "Debug/GameplayDebuggerCategory_Tank.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "TankBase.h"

FGameplayDebuggerCategory_Tank::FGameplayDebuggerCategory_Tank()
{
	bShowOnlyWithDebugActor = false;
	SetDataPackReplication(&DataPack);
	//can be extended to show a certain pawn, but not sure if necessary... see AI Category
}

void FGameplayDebuggerCategory_Tank::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	bool bFallback = false;
	TWeakObjectPtr<ATankBase> Tank = Cast<ATankBase>(DebugActor);
	if(!Tank.IsValid())
	{
		if(OwnerPC != nullptr)
		{
			Tank = Cast<ATankBase>(OwnerPC->GetPawn());
			bFallback = true;
		}
	}
	
	if(Tank.IsValid())
	{
		DataPack.bWasFallBackToPCPawn = bFallback;
		DataPack.ActorName = Tank->GetName();
		DataPack.bIsPlayerControlled = Tank->IsPlayerControlled();
		DataPack.DrivingState = Tank->GetCurrentDrivingState();
		DataPack.VelocityDirection = Tank->GetVelocityDirection();
		DataPack.CurrentSpeed = Tank->GetCurrentSpeed();
		DataPack.CurrentMaxSpeed = Tank->GetCurrentMaxSpeed();
		DataPack.bIsSliding = Tank->GetIsSliding();
		DataPack.SlideDegree = Tank->GetSlideDegree();
	}
	
}

void FGameplayDebuggerCategory_Tank::DrawData(APlayerController* OwnerPC,
	FGameplayDebuggerCanvasContext& CanvasContext)
{
	if(DataPack.bWasFallBackToPCPawn)
	{
		CanvasContext.Printf(TEXT("{red}No Valid DebugActor Selected, falling back to PlayerPawn"));
	}
	CanvasContext.Printf(TEXT("{green}Is PlayerControlled? {yellow}%s"), DataPack.bIsPlayerControlled ? *FString("true") : *FString("false"));
	CanvasContext.Printf(TEXT("{green}Name: {yellow}%s"), *DataPack.ActorName);
	CanvasContext.Printf(TEXT("{green}Current Driving State: {yellow}%s"), *UEnum::GetValueAsString(DataPack.DrivingState));
	CanvasContext.Printf(TEXT("{green}Current Velocity Direction {yellow}%s"), *DataPack.VelocityDirection.ToString());
	CanvasContext.Printf(TEXT("{green}Current Speed: {yellow}%f m/s"), DataPack.CurrentSpeed);
	CanvasContext.Printf(TEXT("{green}Current Max Speed {yellow}%f m/s"), DataPack.CurrentMaxSpeed);
	CanvasContext.Printf(TEXT("{green}Is Sliding? {yellow}%s"), DataPack.bIsSliding ? *FString("true") : *FString("false"));
	if(DataPack.bIsSliding)
	{
		CanvasContext.Printf(TEXT("{green}Current Slide Degree {yellow}%f° Degree"), DataPack.SlideDegree);
	}
	
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Tank::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_Tank());
}

void FGameplayDebuggerCategory_Tank::FRepData::Serialize(FArchive& Ar)
{
	Ar << ActorName;
	Ar << DrivingState;
	Ar << VelocityDirection;
	Ar << CurrentSpeed;
	Ar << CurrentMaxSpeed;
	Ar << bIsSliding;
	Ar << SlideDegree;
}
#endif
