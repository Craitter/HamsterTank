// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/TankProjectileMovementComponent.h"

UTankProjectileMovementComponent::UTankProjectileMovementComponent()
{
	bUpdateOnlyIfRendered = false;
	bInitialVelocityInLocalSpace = true;

	Velocity = FVector(1.f, 0.f, 0.f);

	bWantsInitializeComponent = true;
	bComponentShouldUpdatePhysicsVolume = false;

	bRotationFollowsVelocity = true;
	InitialSpeed = 1100.0f;
	MaxSpeed = 0.0f;
}


void UTankProjectileMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (Velocity.SizeSquared() > 0.f)
	{
		// InitialSpeed > 0 overrides initial velocity magnitude.
		if (InitialSpeed > 0.f)
		{
			Velocity = Velocity.GetSafeNormal() * InitialSpeed;
		}

		if (bInitialVelocityInLocalSpace)
		{
			SetVelocityInLocalSpace(Velocity);
		}

		if (bRotationFollowsVelocity)
		{
			if (UpdatedComponent)
			{
				FRotator DesiredRotation = Velocity.Rotation();
				
				DesiredRotation.Pitch = 0.0f;
				DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
				DesiredRotation.Roll = 0.0f;

				UpdatedComponent->SetWorldRotation(DesiredRotation);
			}
		}

		UpdateComponentVelocity();
		
		if (UpdatedPrimitive && UpdatedPrimitive->IsSimulatingPhysics())
		{
			UpdatedPrimitive->SetPhysicsLinearVelocity(Velocity);
		}
	}
}

void UTankProjectileMovementComponent::UpdateTickRegistration()
{
	Super::UpdateTickRegistration();
}

void UTankProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                     FActorComponentTickFunction* ThisTickFunction)
{

	// Consume PendingForce and reset to zero.
	// At this point, any calls to AddForce() will apply to the next frame.
	PendingForceThisUpdate = PendingForce;
	ClearPendingForce();
	
	if (HasStoppedSimulation() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsValid(UpdatedComponent) || UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	TWeakObjectPtr<AActor> ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner.IsValid() || !CheckStillInWorld())
	{
		return;
	}

	// Initial move state
	FHitResult Hit(1.0f);
	const FVector OldVelocity = Velocity;
	const FVector MoveDelta = ComputeMoveDelta(OldVelocity, DeltaTime);
	FQuat NewRotation = (bRotationFollowsVelocity && !OldVelocity.IsNearlyZero(0.01f)) ? OldVelocity.ToOrientationQuat() : UpdatedComponent->GetComponentQuat();

	if (bRotationFollowsVelocity)
	{
		FRotator DesiredRotation = NewRotation.Rotator();
		DesiredRotation.Pitch = 0.0f;
		DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
		DesiredRotation.Roll = 0.0f;
		NewRotation = DesiredRotation.Quaternion();
	}
	MoveUpdatedComponent(MoveDelta, NewRotation, true, &Hit);

	if(!ActorOwner.IsValid() || HasStoppedSimulation())
	{
		return;
	}

	// Only calculate new velocity if events didn't change it during the movement update.
	if (Velocity == OldVelocity)
	{
		Velocity = ComputeVelocity(Velocity, DeltaTime);				
	}
	if(Hit.bBlockingHit)
	{
		HandleImpact(Hit, DeltaTime, MoveDelta);
	}
	UpdateComponentVelocity();
}

bool UTankProjectileMovementComponent::HandleBlockingHit(const FHitResult& Hit, float DeltaTime,
	const FVector& MoveDelta)
{
	const TWeakObjectPtr<AActor> ActorOwner = UpdatedComponent ? UpdatedComponent->GetOwner() : nullptr;
	if (!CheckStillInWorld() || !ActorOwner.IsValid())
	{
		return true;
	}
	
	HandleImpact(Hit, DeltaTime, MoveDelta);
	
	if (!ActorOwner.IsValid() || HasStoppedSimulation())
	{
		return true;
	}
	return false;
}

void UTankProjectileMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// bool bStopSimulating = false;
	//
	// if (bStopSimulating)
	// {
		StopSimulating(Hit);
	// }
}

void UTankProjectileMovementComponent::SetVelocityInLocalSpace(FVector NewVelocity)
{
	if (UpdatedComponent)
	{
		Velocity = UpdatedComponent->GetComponentToWorld().TransformVectorNoScale(NewVelocity);
	}
}

void UTankProjectileMovementComponent::AddForce(FVector Force)
{
	PendingForce += Force;
}

FVector UTankProjectileMovementComponent::ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const
{
	// Velocity Verlet integration (http://en.wikipedia.org/wiki/Verlet_integration#Velocity_Verlet)
	// The addition of p0 is done outside this method, we are just computing the delta.
	// p = p0 + v0*t + 1/2*a*t^2

	// We use ComputeVelocity() here to infer the acceleration, to make it easier to apply custom velocities.
	// p = p0 + v0*t + 1/2*((v1-v0)/t)*t^2
	// p = p0 + v0*t + 1/2*((v1-v0))*t

	const FVector NewVelocity = ComputeVelocity(InVelocity, DeltaTime);
	const FVector Delta = (InVelocity * DeltaTime) + (NewVelocity - InVelocity) * (0.5f * DeltaTime);
	return Delta;
}

FVector UTankProjectileMovementComponent::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	// v = v0 + a*t
	const FVector Acceleration = ComputeAcceleration(InitialVelocity, DeltaTime);
	const FVector NewVelocity = InitialVelocity + (Acceleration * DeltaTime);

	return LimitVelocity(NewVelocity);
}

FVector UTankProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector Acceleration(FVector::ZeroVector);

	Acceleration += PendingForceThisUpdate;

	return Acceleration;
}

FVector UTankProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const
{
	const float CurrentMaxSpeed = GetMaxSpeed();
	if (CurrentMaxSpeed > 0.f)
	{
		NewVelocity = NewVelocity.GetClampedToMaxSize(CurrentMaxSpeed);
	}
	return ConstrainDirectionToPlane(NewVelocity);
}

bool UTankProjectileMovementComponent::CheckStillInWorld()
{

	if (!IsValid(UpdatedComponent))
	{
		return false;
	}

	const TWeakObjectPtr<UWorld> MyWorld = GetWorld();
	if (!MyWorld.IsValid())
	{
		return false;
	}
	const TWeakObjectPtr<AWorldSettings> WorldSettings = MyWorld->GetWorldSettings( true );
	if (!WorldSettings.IsValid() || !WorldSettings->AreWorldBoundsChecksEnabled())
	{
		return true;
	}
	const TWeakObjectPtr<AActor> ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner.IsValid())
	{
		return false;
	}
	if( ActorOwner->GetActorLocation().Z < WorldSettings->KillZ )
	{
		UDamageType const* DmgType = WorldSettings->KillZDamageType ? WorldSettings->KillZDamageType->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		ActorOwner->FellOutOfWorld(*DmgType);
		return false;
	}

	// Check if box has poked outside the world
	else if( UpdatedComponent && UpdatedComponent->IsRegistered() )
	{
		const FBox&	Box = UpdatedComponent->Bounds.GetBox();
		if(	Box.Min.X < -HALF_WORLD_MAX || Box.Max.X > HALF_WORLD_MAX ||
			Box.Min.Y < -HALF_WORLD_MAX || Box.Max.Y > HALF_WORLD_MAX ||
			Box.Min.Z < -HALF_WORLD_MAX || Box.Max.Z > HALF_WORLD_MAX )
		{
			ActorOwner->OutsideWorldBounds();
			// not safe to use physics or collision at this point
			ActorOwner->SetActorEnableCollision(false);
			FHitResult Hit(1.f);
			StopSimulating(Hit);
			return false;
		}
	}
	return true;
}

void UTankProjectileMovementComponent::StopSimulating(const FHitResult& HitResult)
{
	Velocity = FVector::ZeroVector;
	PendingForce = FVector::ZeroVector;
	PendingForceThisUpdate = FVector::ZeroVector;
	UpdateComponentVelocity();
	SetUpdatedComponent(nullptr);
	OnProjectileStop.Broadcast(HitResult);
}

bool UTankProjectileMovementComponent::HasStoppedSimulation()
{
	return !IsValid(UpdatedComponent) || !IsActive(); 
}

void UTankProjectileMovementComponent::ClearPendingForce(bool bClearImmediateForce)
{
	PendingForce = FVector::ZeroVector;
	if (bClearImmediateForce)
	{
		PendingForceThisUpdate = FVector::ZeroVector;
	}
}

FVector UTankProjectileMovementComponent::GetPendingForce() const
{
	return PendingForce;
}

float UTankProjectileMovementComponent::GetMaxSpeed() const
{
	return MaxSpeed;;
}