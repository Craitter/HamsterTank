// Fill out your copyright notice in the Description page of Project Settings.


#include "TankMovementComponent.h"



UTankMovementComponent::UTankMovementComponent()
{
	MoveComponentFlags = MOVECOMP_NeverIgnoreBlockingOverlaps; //Maybe MOVECOMP_IgnoreBases would be enough not sure yet
	bConstrainToPlane = false;
	bAutoRegisterUpdatedComponent = true;
	NavAgentProps.bCanCrouch = false;
	NavAgentProps.bCanFly = false;
	NavAgentProps.bCanJump = false;
	NavAgentProps.bCanSwim = false;
	NavAgentProps.bCanWalk = true;
	ResetMoveState();
}

void UTankMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	if(ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(PawnOwner) || !IsValid(UpdatedComponent))
	{
		return;
	}
	//see @UFloatingPawnMovement.cpp Tick for use of AI and LocalController

	
	DetermineDrivingState();
	if(GetIsSliding())
	{
		if(!IsStillSliding())
		{
			StopSliding();
		}
	}
	if(DrivingState == EDrivingState::Idle)
	{
		Velocity = FVector::ZeroVector;
	}
	else
	{
		SetDrivingValuesDependingOnState();
		UpdateVelocity(DeltaTime);
	}
	ProcessUserDesiredRotation(DeltaTime);
	
	ConsumeInputVector();
	
//Move
	// bPositionCorrected = false;
	const FVector DeltaVelocity = Velocity * 100 * DeltaTime; //Convert to cm from m/s
	if (DrivingState != EDrivingState::Idle)
	{
		const FQuat Rotation = UpdatedComponent->GetComponentQuat();
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(DeltaVelocity, Rotation, true, Hit);		
		
		if (Hit.IsValidBlockingHit())
		{
			const TWeakObjectPtr<APawn> BumpedPawn = Cast<APawn>(Hit.GetActor());
			if(BumpedPawn.IsValid())
			{
				NotifyBumpedPawn(BumpedPawn.Get());
			}
			else
			{
				StartSliding(Hit.ImpactNormal);
				
				HandleImpact(Hit, DeltaTime, DeltaVelocity);
				// Try to slide the remaining distance along the surface.
				SlideAlongSurface(DeltaVelocity, 1.f-Hit.Time, Hit.Normal, Hit, true);
				// UE_LOG(LogTemp, Warning , TEXT("sliding"));
			}
		}
		else
		{
			// UE_LOG(LogTemp, Warning , TEXT("NoSliding"));
			StopSliding();
		}
	}
	UpdateComponentVelocity();
}

void UTankMovementComponent::NotifyBumpedPawn(APawn* BumpedPawn)
{
	Super::NotifyBumpedPawn(BumpedPawn);
}

void UTankMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
	// UE_LOG(LogTemp, Warning , TEXT("Impact"));
	//TODO: CameraShakeShit
	
}

void UTankMovementComponent::UpdateComponentVelocity()
{
	if ( UpdatedComponent )
	{
		UpdatedComponent->ComponentVelocity = Velocity * 100;
	}
}

// bool UTankMovementComponent::ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit,
// 	const FQuat& NewRotation)
// {
// 	// bPositionCorrected |=
// 		return Super::ResolvePenetrationImpl(Adjustment, Hit, NewRotation);
// 	// return bPositionCorrected;
// }

void UTankMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTankMovementComponent::DetermineDrivingState()
{
	const FVector DesiredDirectionVector = PawnOwner->GetActorForwardVector() * GetPendingInputVector().X;
	const FVector VelocityDirection = Velocity.IsNearlyZero() ? DesiredDirectionVector : Velocity.GetSafeNormal();
	//This is important so we dont stay stuck on Idle because DOTProduct will always return 0 with one Param being Zero
	
	const float DotProduct = FVector::DotProduct(DesiredDirectionVector, VelocityDirection);
	if(DotProduct <= UE_THRESH_NORMALS_ARE_ORTHOGONAL && DotProduct >= -UE_THRESH_NORMALS_ARE_ORTHOGONAL)
	{
		if(Velocity.SizeSquared() < UE_FLOAT_NORMAL_THRESH)
		{
			DrivingState = EDrivingState::Idle;
		}
		else
		{
			DrivingState = EDrivingState::Neutral;
		}
	}
	else if(DotProduct > 0.0f)
	{
		if(GetPendingInputVector().X > 0.0f)
		{
			DrivingState = EDrivingState::Forward;
		}
		else
		{
			DrivingState = EDrivingState::Backward;
		}
	}
	else // (DotProduct < 0.0f)
	{
		DrivingState = EDrivingState::Breaking;
	}
}

void UTankMovementComponent::SetForwardDrivingValues()
{
	CurrentMaxSpeed = ForwardMaxSpeed * GetSlideVelocityRatio();
	CurrentDrivingForce = Mass * ForwardConstantAcceleration;
}

void UTankMovementComponent::SetBackwardDrivingValues()
{
	CurrentMaxSpeed = BackwardMaxSpeed * GetSlideVelocityRatio();
	CurrentDrivingForce = Mass * BackwardConstantAcceleration;
}

void UTankMovementComponent::SetBreakingDrivingValues()
{
	CurrentMaxSpeed = ForwardMaxSpeed;
	const float InBreakingDistance = BreakingDistance * ( Velocity.Size() / ForwardMaxSpeed);
	const float BreakingAcceleration = Velocity.SizeSquared() / (2 * InBreakingDistance);
	CurrentDrivingForce = Mass * BreakingAcceleration;
}

float UTankMovementComponent::GetSlideVelocityRatio() const
{
	const float SlideRatio = GetIsSliding() ? fabs(fabs(FVector::DotProduct(PawnOwner->GetActorForwardVector(), BlockedDirection)) -1) : 1.0f;
	return  FMath::Clamp(SlideRatio, MinVelocityRatioWhenSliding, 1.0f);
}

bool UTankMovementComponent::IsStillSliding()
{
	//Maybe some of this can be removed because when there is no blocking hit we stop sliding as well...
	if(DrivingState == EDrivingState::Idle || BlockedDirection.IsNearlyZero())
	{
		return false;
	}
	const float BlockingDirectionDelta = FVector::DotProduct(BlockedDirection, Velocity.GetSafeNormal());
	if(BlockingDirectionDelta < 0.0f)
	{
		return false;
	}
	const float InputToBlockingDirectionDelta = FVector::DotProduct(PawnOwner->GetActorForwardVector() * GetPendingInputVector().X, BlockedDirection);
	if(InputToBlockingDirectionDelta < -UE_FLOAT_NORMAL_THRESH && InputToBlockingDirectionDelta < -DeltaRatioWallPawnVelocityOverride)
	{
		Velocity = FVector::ZeroVector;
		//We Reset Driving STate so lets start over
		DetermineDrivingState();
		return false;
	}
	return true;
}

void UTankMovementComponent::StartSliding(const FVector& ImpactNormal)
{
	bIsSliding = true;
	BlockedDirection = ImpactNormal * -1;
}

bool UTankMovementComponent::GetIsSliding() const
{
	return bIsSliding;
}

void UTankMovementComponent::StopSliding()
{
	bIsSliding = false;
	BlockedDirection = FVector::ZeroVector;
}

void UTankMovementComponent::SetDrivingValuesDependingOnState()
{
	switch (DrivingState)
	{
	case EDrivingState::Idle:
	case EDrivingState::Neutral:
	case EDrivingState::Forward:
		SetForwardDrivingValues();
		break;
	case EDrivingState::Backward:
		SetBackwardDrivingValues();
		break;
	case EDrivingState::Breaking:
		SetBreakingDrivingValues();
		break;
	default: ;
	}
}

void UTankMovementComponent::UpdateVelocity(const float InDeltaTime)
{
	FVector Force = PawnOwner->GetActorForwardVector() * CurrentDrivingForce * GetPendingInputVector().X;

	//Applying Resistant Forces to the Engine Force
	Force += GetAirResistance(); 
	Force += GetRollingResistance(); 

	//Create the Velocity / Update Velocity
	const FVector Acceleration = Force / Mass;
	Velocity += Acceleration * InDeltaTime; // makes m/s from the acceleration which is m/s^2
}


FVector UTankMovementComponent::GetAirResistance() const
{
	const float InDragCoefficient = CurrentDrivingForce / (CurrentMaxSpeed * CurrentMaxSpeed);
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * InDragCoefficient;
}

FVector UTankMovementComponent::GetRollingResistance() const
{
	const float AccelerationDueToGravity = GetWorld()->GetGravityZ() / -100.0f; //Convert to cm
	const float NormalForce = AccelerationDueToGravity * Mass;
	return -Velocity.GetSafeNormal() * NormalForce *  RollingResistanceCoefficient;
}

void UTankMovementComponent::ProcessUserDesiredRotation(float InDeltaTime)
{
	FQuat RotationDelta;
	if(DrivingState == EDrivingState::Idle)
	{
		const float RotationAngle = MaxFixedTurningDegree * InDeltaTime * GetPendingInputVector().Y;		
		RotationDelta = FQuat(PawnOwner->GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));
	}
	else
	{
		const float DeltaLocation = FVector::DotProduct(PawnOwner->GetActorForwardVector(), Velocity) * InDeltaTime;
		const float RotationAngle =  DeltaLocation / MinTurningRadius * GetPendingInputVector().Y;
		RotationDelta = FQuat(PawnOwner->GetActorUpVector(), RotationAngle);
	}
	//Adjusting Tank Rotation and rotating the Velocity to match the direction
	Velocity = RotationDelta.RotateVector(Velocity);
	PawnOwner->AddActorWorldRotation(RotationDelta, true);
	
}