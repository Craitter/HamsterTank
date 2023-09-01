// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PredictMovementAsset.h"


FVector UPredictMovementAsset::GetPredictedMovementLocation(
	const TWeakObjectPtr<APawn> TargetPawn, const float SecondsToPredict, const bool bPredictRotation)
{
	check(GetWorld())
	if(!TargetPawn.IsValid() || SecondsToPredict < 0.0f)
	{
		return FVector::ZeroVector;
	}

	const float CurrentTimeInSeconds = GetWorld()->GetTimeSeconds();
	float DeltaTime = CurrentTimeInSeconds - LastUpdateTimeInSeconds;
	if(DeltaTime > MaxUpdateTimeBetweenRequests)
	{
		RefreshPredictedMovement(TargetPawn, SecondsToPredict);
		DeltaTime = 0.0f;
		LastUpdateTimeInSeconds = CurrentTimeInSeconds;
	}
	
	return GetPredictedLocation(TargetPawn, SecondsToPredict + DeltaTime, bPredictRotation);
}

void UPredictMovementAsset::RefreshPredictedMovement(TWeakObjectPtr<APawn> TargetPawn, float SecondsToPredict)
{
	// const TWeakObjectPtr<UTankMovementComponent> TankMovementComponent = Cast<UTankMovementComponent>(TargetPawn->GetMovementComponent());
	// if(!TankMovementComponent.IsValid())
	// {
	// 	return;
	// }
	//
	// FVector TempVelocity = TankMovementComponent->Velocity;
	// const float Steps = Seconds / 0.05;
	// float Step = 0;
	// const float TimeSizePerStep = Seconds / Steps;
	//
	// FVector ForwardTemp = TargetPawn->GetActorForwardVector();
	// FVector DeltaLocation = FVector::ZeroVector;
	// do
	// {
	// 	FVector Force = ForwardTemp * CurrentDrivingForce * GetLastInputVector().X;
	// 	
	// 	
	// 	Force += GetAirResistance(TempVelocity);
	// 	Force += GetRollingResistance(TempVelocity);
	// 	//Create the Velocity / Update Velocity
	// 	const FVector Acceleration = Force / Mass;
	// 	TempVelocity += Acceleration * TimeSizePerStep; // makes m/s from the acceleration which is m/s^2
	// 	
	// 	if(DrivingState != EDrivingState::DS_Idle && bPredictRotation)
	// 	{
	// 		const float DeltaTurn = FVector::DotProduct(ForwardTemp, TempVelocity) * TimeSizePerStep;
	// 		const float RotationAngle = DeltaTurn / GetMinTurningRadius(TempVelocity.Size()) * GetLastInputVector().Y;
	// 		FQuat RotationDelta = FQuat(PawnOwner->GetActorUpVector(), RotationAngle);
	// 		TempVelocity = RotationDelta.RotateVector(TempVelocity);
	// 		ForwardTemp = RotationDelta.RotateVector(ForwardTemp);
	// 	}
	// 	
	// 	DeltaLocation += TempVelocity * 100.0f * TimeSizePerStep;
	// 	Step++;
	// }
	// while (Step <= Steps);
	// 	
	//
	// const FVector StartLocation = UpdatedComponent->GetComponentLocation();
	//
	// return  StartLocation + DeltaLocation;
}

FVector UPredictMovementAsset::GetPredictedLocation(TWeakObjectPtr<APawn> TargetPawn, float SecondsInFuture, bool bPredictRotation)
{
	const TArray<FPredictedMovement>* PredictedMovements = PawnMap.Find(TargetPawn);
	if(PredictedMovements != nullptr)
	{
		const TArray<FPredictedMovement>& Movements = *PredictedMovements;
		const int32 Index = FMath::RoundToInt32(SecondsInFuture / TimePerStep) - 1;
		if(Movements.IsValidIndex(Index))
		{
		 	return bPredictRotation ? Movements[Index].RotatedLocation : Movements[Index].UnRotatedLocation;
		}
	}
	return FVector::ZeroVector;
}
