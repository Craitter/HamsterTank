// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TankMovementComponent.h"


#if ENABLE_DRAW_DEBUG && !NO_CVARS
#include "Components/SphereComponent.h"
#endif

#if ENABLE_DRAW_DEBUG && !NO_CVARS
static TAutoConsoleVariable<bool> CVarShowAllTankMovement(
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

}

void UTankMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	if(ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	//see @UFloatingPawnMovement.cpp Tick for use of AI and LocalController

	
	DetermineDrivingState();
	if(GetIsSliding())
	{
		if(!IsStillSliding())
		{
			StopSliding();
		}
	}
	if(DrivingState == EDrivingState::DS_Idle)
	{
		Velocity = FVector::ZeroVector;
	}
	else
	{
		SetDrivingValuesDependingOnState();
		ComputeVelocity(DeltaTime, Velocity);
	}
	ProcessUserDesiredRotation(DeltaTime);
	
#if ENABLE_DRAW_DEBUG && !NO_CVARS
	DrawTickDebug();
#endif
	
	ConsumeInputVector();
	
//Move
	// bPositionCorrected = false;
	const FVector DeltaVelocity = Velocity * 100 * DeltaTime; //Convert to cm from m/s
	if (DrivingState != EDrivingState::DS_Idle)
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
				// UE_LOG(LogTemp, Warning , TEXT("sliding"));,,,,,,
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

bool UTankMovementComponent::ShouldSkipUpdate(float DeltaTime) const
{
	return Super::ShouldSkipUpdate(DeltaTime) || !IsValid(PawnOwner) || !IsValid(UpdatedComponent);
}

FVector UTankMovementComponent::PredictLocationAfterSeconds(const float Seconds,
                                                            bool bPredictRotation) const
{
	//todo do this
	if(!IsValid(UpdatedComponent) || !IsValid(PawnOwner))
	{
		UE_LOG(LogTemp, Warning , TEXT("Updated Component is Zero"));
		return FVector::ZeroVector;
	}
	FVector TempVelocity = Velocity;
	const float Steps = Seconds / 0.05;
	float Step = 0;
	const float TimeSizePerStep = Seconds / Steps;

	FVector ForwardTemp = PawnOwner->GetActorForwardVector();
	FVector DeltaLocation = FVector::ZeroVector;
	do
	{
		FVector Force = ForwardTemp * CurrentDrivingForce * GetLastInputVector().X;
		
		
		Force += GetAirResistance(TempVelocity);
		Force += GetRollingResistance(TempVelocity);
		//Create the Velocity / Update Velocity
		const FVector Acceleration = Force / Mass;
		TempVelocity += Acceleration * TimeSizePerStep; // makes m/s from the acceleration which is m/s^2
		
		if(DrivingState != EDrivingState::DS_Idle && bPredictRotation)
		{
			const float DeltaTurn = FVector::DotProduct(ForwardTemp, TempVelocity) * TimeSizePerStep;
			const float RotationAngle = DeltaTurn / GetMinTurningRadius(TempVelocity.Size()) * GetLastInputVector().Y;
			FQuat RotationDelta = FQuat(PawnOwner->GetActorUpVector(), RotationAngle);
			TempVelocity = RotationDelta.RotateVector(TempVelocity);
			ForwardTemp = RotationDelta.RotateVector(ForwardTemp);
		}
		
		DeltaLocation += TempVelocity * 100.0f * TimeSizePerStep;
		Step++;
	}
	while (Step <= Steps);
		
	
	const FVector StartLocation = UpdatedComponent->GetComponentLocation();
	
	return  StartLocation + DeltaLocation;
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
#if ENABLE_DRAW_DEBUG && !NO_CVARS
	if(CVarShowAllTankMovement.AsVariable() != nullptr)
	{
		CVarShowAllTankMovement->AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UTankMovementComponent::OnToggleAllDebug));
	}
#endif
}

void UTankMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
// #if ENABLE_DRAW_DEBUG && !NO_CVARS
// 	if(CVarShowAllTankMovement.AsVariable() != nullptr)
// 	{
// 		CVarShowAllTankMovement->AsVariable()->SetOnChangedCallback();
// 	}
// #endif
	Super::EndPlay(EndPlayReason);
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
			DrivingState = EDrivingState::DS_Idle;
		}
		else
		{
			DrivingState = EDrivingState::DS_Neutral;
		}
	}
	else if(DotProduct > 0.0f)
	{
		if(GetPendingInputVector().X > 0.0f)
		{
			DrivingState = EDrivingState::DS_Forward;
		}
		else
		{
			DrivingState = EDrivingState::DS_Backward;
		}
	}
	else // (DotProduct < 0.0f)
	{
		DrivingState = EDrivingState::DS_Breaking;
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
	float BreakingAcceleration = Velocity.SizeSquared() / (2 * InBreakingDistance);
	BreakingAcceleration = BreakingAcceleration < MinBreakingDeceleration ? MinBreakingDeceleration : BreakingAcceleration;
	CurrentDrivingForce = Mass * BreakingAcceleration;
}

float UTankMovementComponent::GetSlideVelocityRatio() const
{
	const float SlideRatio = GetIsSliding() ? fabs(fabs(FVector::DotProduct(PawnOwner->GetActorForwardVector(), BlockedDirection)) -1) : 1.0f;
	return  FMath::Clamp(SlideRatio, MinVelocityRatioWhenSliding, 1.0f);
}

bool UTankMovementComponent::IsStillSliding()
{ //Todo make single functions for each check
	//Maybe some of this can be removed because when there is no blocking hit we stop sliding as well...
	if(DrivingState == EDrivingState::DS_Idle || BlockedDirection.IsNearlyZero())
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
	case EDrivingState::DS_Idle:
	case EDrivingState::DS_Neutral:
	case EDrivingState::DS_Forward:
		SetForwardDrivingValues();
		break;
	case EDrivingState::DS_Backward:
		SetBackwardDrivingValues();
		break;
	case EDrivingState::DS_Breaking:
		SetBreakingDrivingValues();
		break;
	default: ;
	}
}

void UTankMovementComponent::ComputeVelocity(const float InDeltaTime, FVector& InVelocity) const
{
	FVector Force = PawnOwner->GetActorForwardVector() * CurrentDrivingForce * GetPendingInputVector().X;

	//Applying Resistant Forces to the Engine Force
	Force += GetAirResistance(InVelocity); 
	Force += GetRollingResistance(InVelocity); 

	//Create the Velocity / Update Velocity
	const FVector Acceleration = Force / Mass;
	InVelocity += Acceleration * InDeltaTime; // makes m/s from the acceleration which is m/s^2
}


FVector UTankMovementComponent::GetAirResistance(const FVector& InVelocity) const
{
	const float InDragCoefficient = CurrentDrivingForce / (CurrentMaxSpeed * CurrentMaxSpeed);
	return -InVelocity.GetSafeNormal() * InVelocity.SizeSquared() * InDragCoefficient;
}

FVector UTankMovementComponent::GetRollingResistance(const FVector& InVelocity) const
{
	const float AccelerationDueToGravity = GetWorld()->GetGravityZ() / -100.0f; //Convert to cm
	const float NormalForce = AccelerationDueToGravity * Mass;
	return -InVelocity.GetSafeNormal() * NormalForce *  RollingResistanceCoefficient;
}

float UTankMovementComponent::GetMinTurningRadius(float Speed) const
{
	if(SpeedTurningCurve.GetRichCurveConst() != nullptr)
	{
		return SpeedTurningCurve.GetRichCurveConst()->Eval(Speed);
	}
	else
	{
		UE_LOG(LogTemp, Error , TEXT("%s() SpeedTurningCurve is NULL, fill out BP first"), *FString(__FUNCTION__));
		return -1.0f;
	}
}

float UTankMovementComponent::GetMass() const
{
	return Mass;
}

float UTankMovementComponent::GetCurrentDrivingForce() const
{
	return CurrentDrivingForce;
}

void UTankMovementComponent::ProcessUserDesiredRotation(float InDeltaTime)
{
	FQuat RotationDelta;
	if(DrivingState == EDrivingState::DS_Idle)
	{
		const float RotationAngle = MaxFixedTurningDegree * InDeltaTime * GetPendingInputVector().Y;		
		RotationDelta = FQuat(PawnOwner->GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));
	}
	else
	{
		const float MinTurningRadius = GetMinTurningRadius(Velocity.Size());
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
	const bool NewFlag = CVarShowAllTankMovement->GetBool();
	CVarShowCollider->Set(NewFlag);
	CVarShowImpact->Set(NewFlag);
	CVarShowInputDirection->Set(NewFlag);
	CVarShowTurningCircle->Set(NewFlag);
	CVarShowVelocityDirection->Set(NewFlag);
}

void UTankMovementComponent::DrawTickDebug() const
{
	FVector StartLocation;
	FVector EndLocation;
	
	if(CVarShowVelocityDirection->GetBool())
	{
		constexpr float ZOffset = 100.0f;
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
		const FColor PressedColor = FColor::White;
		const FColor ReleasedColor = FColor(FLinearColor::Gray.ToFColor(true));
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
		constexpr float Offset = 100.0f;
		constexpr float Length = 50.0f;
		constexpr float ArrowSize = 30.0f;
		DrawDebugDirectionalArrow(GetWorld(), StartLocation + PawnOwner->GetActorForwardVector() * Offset, StartLocation + PawnOwner->GetActorForwardVector() * (Offset + Length), ArrowSize, ForwardArrowColor, false, -1, 4.0f, 10.0f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation - PawnOwner->GetActorForwardVector() * Offset, StartLocation - PawnOwner->GetActorForwardVector() * (Offset + Length), ArrowSize, BackwardArrowColor, false, -1, 4.0f, 10.0f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation + PawnOwner->GetActorRightVector() * Offset, StartLocation + PawnOwner->GetActorRightVector() * (Offset + Length), ArrowSize, RightArrowColor, false, -1, 4.0f, 10.0f);
		DrawDebugDirectionalArrow(GetWorld(), StartLocation - PawnOwner->GetActorRightVector() * Offset, StartLocation - PawnOwner->GetActorRightVector() * (Offset + Length), ArrowSize, LeftArrowColor, false, -1, 4.0f, 10.0f);
	}
	if(CVarShowCollider->GetBool())
	{
		const TWeakObjectPtr<USphereComponent> Shape = Cast<USphereComponent>(UpdatedComponent);
		if(Shape.IsValid())
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), Shape->GetScaledSphereRadius(), 10.0f, FColor::Green, false, -1, 1);
		}
	}
}

#endif

