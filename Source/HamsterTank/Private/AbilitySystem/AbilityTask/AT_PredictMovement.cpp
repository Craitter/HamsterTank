// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AT_PredictMovement.h"

#include "Actors/TankBase.h"
#include "Components/TankMovementComponent.h"

UAT_PredictMovement* UAT_PredictMovement::PredictMovement(UGameplayAbility* OwningAbility, const FName TaskInstanceName,
                                                          ATankBase* InOwningTank, const bool bShouldPredictRotation, const float& InDeltaTime, const float& InSubSteppingRate, const float& InSecondsToPredict)
{
	UAT_PredictMovement* NewTask = NewAbilityTask<UAT_PredictMovement>(OwningAbility, TaskInstanceName);
	if(NewTask != nullptr)
	{
		NewTask->OwningTank = InOwningTank;
		NewTask->bPredictRotation = bShouldPredictRotation;
		NewTask->DeltaTime = InDeltaTime;
		NewTask->SubSteppingRate = InSubSteppingRate;
		NewTask->SecondsToPredict = InSecondsToPredict;
	}
	return NewTask;
}

void UAT_PredictMovement::Activate()
{
	Super::Activate();
	
	check(GetWorld())
	
	
	GetWorld()->GetTimerManager().SetTimer(PredictHandle, this, &ThisClass::PredictMovement, DeltaTime, true);

	if(OwningTank.IsValid())
	{
		MovementComponent = OwningTank->GetTankMovement();
	}
	PredictMovement();
}

float UAT_PredictMovement::GetDeltaTime() const
{
	check(GetWorld())
	
	
	return DeltaTime - GetWorld()->GetTimerManager().GetTimerRemaining(PredictHandle);
}

void UAT_PredictMovement::PredictMovement()
{
	if(!MovementComponent.IsValid() || !OwningTank.IsValid() || !OnMovementPredicted.IsBound() || !OnPredictionStarted.IsBound())
	{
		EndTask();
		return;
	}
	OnPredictionStarted.Execute();
	//
	//Adding DeltaTime as safety:
	//Seconds to Predict 3...
	//DeltaTime = 1
	//Tower1 Requestes 0.0001s before Timer End, it wants the location after 3 seconds... results in Requested Time being 4.. That would be bad
	const float Steps = (SecondsToPredict + DeltaTime) / SubSteppingRate;
	float Step = 0;
	
	const FVector StartLocation = OwningTank->GetActorLocation();
	FVector PredictedVelocity = MovementComponent->Velocity;
	FVector DynamicFwdVector = OwningTank->GetActorForwardVector();
	FVector DeltaLocation = FVector::ZeroVector;
	do
	{
		FVector Force = DynamicFwdVector * MovementComponent->GetCurrentDrivingForce() * MovementComponent->GetLastInputVector().X;
		
		
		Force += MovementComponent->GetAirResistance(PredictedVelocity);
		Force += MovementComponent->GetRollingResistance(PredictedVelocity);
		//Create the Velocity / Update Velocity
		const FVector Acceleration = Force / MovementComponent->GetMass();
		PredictedVelocity += Acceleration * SubSteppingRate; // makes m/s from the acceleration which is m/s^2
		
		if(bPredictRotation && OwningTank->GetCurrentDrivingState() != EDrivingState::DS_Idle)
		{
			const float DeltaTurn = FVector::DotProduct(DynamicFwdVector, PredictedVelocity) * SubSteppingRate;
			const float RotationAngle = DeltaTurn / MovementComponent->GetMinTurningRadius(PredictedVelocity.Size()) * MovementComponent->GetLastInputVector().Y;
			FQuat RotationDelta = FQuat(OwningTank->GetActorUpVector(), RotationAngle);
			PredictedVelocity = RotationDelta.RotateVector(PredictedVelocity);
			DynamicFwdVector = RotationDelta.RotateVector(DynamicFwdVector);
		}
		
		DeltaLocation += PredictedVelocity * 100.0f * SubSteppingRate;
		Step++;
		OnMovementPredicted.Execute(StartLocation + DeltaLocation);
	}
	while (Step <= Steps);
		
	

	
	
}
