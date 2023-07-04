// Fill out your copyright notice in the Description page of Project Settings.


#include "TankMovementComponent.h"


#if ENABLE_DRAW_DEBUG && !NO_CVARS
#include "Components/SphereComponent.h"
#endif

#if ENABLE_DRAW_DEBUG && !NO_CVARS
static TAutoConsoleVariable<bool> CVarShowAll(
	TEXT("Tank.Movement.Debug.All"),
	false,
	TEXT("Shows all available Debug options"),
	ECVF_Default);

// static TAutoConsoleVariable<bool> CVarShowPlayer(
// 	TEXT("Tank.Movement.Debug.Player"),
// 	false,
// 	TEXT("Shows all available Debug options for the Player"),
// 	ECVF_Default);
//
// static TAutoConsoleVariable<bool> CVarShowAI(
// 	TEXT("Tank.Movement.Debug.AI"),
// 	false,
// 	TEXT("Shows all available Debug options for the NonPlayer"),
// 	ECVF_Default);

static TAutoConsoleVariable<bool> CVarShowImpact(
	TEXT("Tank.Movement.Debug.Impact"),
	false,
	TEXT("Shows Impacts that happend while moving"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarShowVelocityDirection(
	TEXT("Tank.Movement.Debug.VelocityDirection"),
	false,
	TEXT("Draws an Arrow which faces in Direction of the Velocity"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarShowInputDirection(
	TEXT("Tank.Movement.Debug.InputDirection"),
	false,
	TEXT("Draws 4 Arrows and shows which input Directions are pressed"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarShowTurningCircle(
	TEXT("Tank.Movement.Debug.TurningCircle"),
	false,
	TEXT("Draws a circle when there is Y input which displays the turning circle"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarShowCollider(
	TEXT("Tank.Movement.Debug.Collider"),
	false,
	TEXT("Draws the CollisionSphere of the Pawn"),
	ECVF_Default);

// static TAutoConsoleVariable<bool> CVarShowImpact(
// 	TEXT("Tank.Movement.Debug.ShowImpact"),
// 	false,
// 	TEXT("Shows Impacts that happend while moving"),
// 	ECVF_Default);


#endif

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
#if ENABLE_DRAW_DEBUG && !NO_CVARS
	CVarShowAll->AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UTankMovementComponent::OnToggleAllDebug));
#endif
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
	
#if ENABLE_DRAW_DEBUG && !NO_CVARS
	FVector StartLocation;
	FVector EndLocation;
	
	if(CVarShowVelocityDirection->GetBool())
	{
		float ZOffset = 100.0f;
		if(Velocity.IsNearlyZero())
		{
			StartLocation = GetActorLocation();
			StartLocation.Z += ZOffset;
			DrawDebugPoint(GetWorld(), StartLocation, 10.0f, FColor::Blue, false, -1, 5.0f);
		
		}
		else
		{
			StartLocation = GetActorLocation() - Velocity.GetSafeNormal() * 20;
			StartLocation.Z += ZOffset;
			EndLocation = GetActorLocation() + Velocity.GetSafeNormal() * 20;
			EndLocation.Z += ZOffset;
			DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 30.0f, FColor::Blue, false, -1, 5.0f, 10.0f);
		}

	}
	if(CVarShowInputDirection->GetBool())
	{
		FColor PressedColor = FColor::White;
		FColor ReleasedColor = FColor(FLinearColor::Gray.ToFColor(true));
		FColor RightArrowColor = ReleasedColor;
		FColor ForwardArrowColor = ReleasedColor;
		FColor LeftArrowColor = ReleasedColor;
		FColor BackwardArrowColor = ReleasedColor;
		if(!GetPendingInputVector().IsNearlyZero())
		{
			if(GetPendingInputVector().X < -UE_FLOAT_NORMAL_THRESH)
			{
				BackwardArrowColor = PressedColor;
			}
			else if(GetPendingInputVector().X > UE_FLOAT_NORMAL_THRESH)
			{
				ForwardArrowColor = PressedColor;
			}

			if(GetPendingInputVector().Y < -UE_FLOAT_NORMAL_THRESH)
			{
				LeftArrowColor = PressedColor;
			}
			else if(GetPendingInputVector().Y > UE_FLOAT_NORMAL_THRESH)
			{
				RightArrowColor = PressedColor;
			}

			
		}
		StartLocation = GetActorLocation();
		EndLocation = StartLocation;
		float Offset = 100.0f;
		float Length = 50.0f;
		float ArrowSize = 30.0f;
		DrawDebugDirectionalArrow(GetWorld(), StartLocation + PawnOwner->GetActorForwardVector() * Offset, StartLocation + PawnOwner->GetActorForwardVector() * (Offset + Length), ArrowSize, ForwardArrowColor, false, -1, 4.0f, 10.0f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation - PawnOwner->GetActorForwardVector() * Offset, StartLocation - PawnOwner->GetActorForwardVector() * (Offset + Length), ArrowSize, BackwardArrowColor, false, -1, 4.0f, 10.0f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation + PawnOwner->GetActorRightVector() * Offset, StartLocation + PawnOwner->GetActorRightVector() * (Offset + Length), ArrowSize, RightArrowColor, false, -1, 4.0f, 10.0f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation - PawnOwner->GetActorRightVector() * Offset, StartLocation - PawnOwner->GetActorRightVector() * (Offset + Length), ArrowSize, LeftArrowColor, false, -1, 4.0f, 10.0f);
	}
	if(CVarShowCollider->GetBool())
	{
		TWeakObjectPtr<USphereComponent> Shape = Cast<USphereComponent>(UpdatedComponent);
		if(Shape.IsValid())
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), Shape->GetScaledSphereRadius(), 10.0f, FColor::Green, false, -1, 1);
		}
		
	}
	//VelocityDirection
	//MaxSpeed
	//CurrentSpeed
	//DrivingState
	//Sliding
	//Resistance
#endif

	
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

#if ENABLE_DRAW_DEBUG && !NO_CVARS
	if(CVarShowImpact->GetBool())
	{
		DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.0f, FColor::Yellow, false, 0.5f, 0.0f);
	}
#endif
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
	const float InBreakingDistance = (BreakingDistance / 100) * ( Velocity.Size() / ForwardMaxSpeed);
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
		const float RotationAngle = DeltaLocation / MinTurningRadius * GetPendingInputVector().Y;
		RotationDelta = FQuat(PawnOwner->GetActorUpVector(), RotationAngle);

#if ENABLE_DRAW_DEBUG && !NO_CVARS
		if(CVarShowTurningCircle->GetBool() && fabs(GetPendingInputVector().Y) > UE_FLOAT_NORMAL_THRESH)
		{
			const FVector Direction = GetPendingInputVector().Y > 0.0f ? PawnOwner->GetActorRightVector() : -PawnOwner->GetActorRightVector();
			const FVector Location = GetActorLocation() +  Direction * MinTurningRadius * 100;
			DrawDebugCircle(GetWorld(), Location, MinTurningRadius * 100, 100.0f, FColor::Black, false, -1, 2.0f, 8.0f, FVector(0, 1, 0), FVector(1, 0, 0));
		}
	
#endif
		
	}
	//Adjusting Tank Rotation and rotating the Velocity to match the direction
	Velocity = RotationDelta.RotateVector(Velocity);
	PawnOwner->AddActorWorldRotation(RotationDelta, true);
	
}

EDrivingState UTankMovementComponent::GetCurrentDrivingState() const
{
	return DrivingState;
}

FVector UTankMovementComponent::GetVelocityDirection() const
{
	return Velocity.GetSafeNormal();
}

float UTankMovementComponent::GetCurrentSpeed() const
{
	return Velocity.Size();
}

float UTankMovementComponent::GetCurrentMaxSpeed() const
{
	return CurrentMaxSpeed;
}

float UTankMovementComponent::GetSlideDegree() const
{
	if(GetIsSliding())
	{
		const float SlideRatio = GetIsSliding() ? fabs(fabs(FVector::DotProduct(PawnOwner->GetActorForwardVector(), BlockedDirection)) -1) : 1.0f;
		return FMath::RadiansToDegrees(acos(SlideRatio));
	}
	return -1.0f;
}

bool UTankMovementComponent::GetIsSliding() const
{
	return bIsSliding;
}

#if ENABLE_DRAW_DEBUG && !NO_CVARS

void UTankMovementComponent::OnToggleAllDebug(IConsoleVariable* ConsoleVariable)
{
	const bool NewFlag = CVarShowAll->GetBool();
	CVarShowCollider->Set(NewFlag);
	CVarShowImpact->Set(NewFlag);
	CVarShowInputDirection->Set(NewFlag);
	CVarShowTurningCircle->Set(NewFlag);
	CVarShowVelocityDirection->Set(NewFlag);
}

#endif

