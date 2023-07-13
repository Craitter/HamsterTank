// Fill out your copyright notice in the Description page of Project Settings.



#include "TankBase.h"

#include "TankMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"



// Sets default values
ATankBase::ATankBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	if(!ensure(IsValid(Sphere))) return;
	Base = CreateDefaultSubobject<USkeletalMeshComponent>("TankBase");
	if(!ensure(IsValid(Base))) return;
	Tower = CreateDefaultSubobject<USkeletalMeshComponent>("TankTower");
	if(!ensure(IsValid(Tower))) return;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	if(!ensure(IsValid(SpringArmComponent))) return;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	if(!ensure(IsValid(Camera))) return;
	TankMovement = CreateDefaultSubobject<UTankMovementComponent>("TankMovement");
	if(!ensure(IsValid(TankMovement))) return;
	
	SetRootComponent(Sphere);
	Base->SetupAttachment(Sphere);
	Tower->SetupAttachment(Sphere);
	Tower->SetUsingAbsoluteRotation(true);
	SpringArmComponent->SetupAttachment(Tower);
	SpringArmComponent->bDoCollisionTest = false;
	//Todo:Set more default values so we dont lose them when renaming for example
	Camera->SetupAttachment(SpringArmComponent);
}



// Called when the game starts or when spawned
void ATankBase::BeginPlay()
{
	Super::BeginPlay();
	
}


// void ATankBase::UpdateTowerRotation(float DeltaTime) const
// {
// 	if(IsValid(Tower))
// 	{
// 		const FVector TowerLocation = Tower->GetComponentLocation();
// 		const FVector AimTargetDirection = DesiredTowerAimLocation - TowerLocation;
// 		const FQuat ShortestRotation = FQuat::FindBetweenVectors(Tower->GetForwardVector(), AimTargetDirection);
// 		const float DeltaDegree = FMath::RadiansToDegrees(ShortestRotation.GetAngle());
// 		const float AngleRatio = FMath::Clamp( (DeltaTime * MaxTowerTurningDegreePerSecond) / DeltaDegree, 0.0f, 1.0f);
// 		const FQuat DeltaRotation = FQuat::Slerp(Tower->GetComponentRotation().Quaternion(), AimTargetDirection.Rotation().Quaternion(), AngleRatio);
// 		Tower->SetWorldRotation(DeltaRotation);
// 	}
// }

// Called every frame
void ATankBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// UpdateTowerRotation(DeltaTime);
}


// void ATankBase::RequestAimAtTarget(const FVector& TargetLocation)
// {
// 	if(IsValid(Tower) && !bIsTargetLocationLocked)
// 	{
// 		DesiredTowerAimLocation = TargetLocation;
// 		DesiredTowerAimLocation.Z = Tower->GetComponentLocation().Z;
// 	}
// }

void ATankBase::RequestFire() const
{
	if(IsValid(Tower))
	{
		const FVector StartLocation = Tower->GetComponentLocation();
		constexpr float Distance =  1000.0f;
		const FVector EndLocation = StartLocation + Tower->GetForwardVector() * Distance;
		DrawDebugSphere(GetWorld(), EndLocation, 10.0f, 10.0f,  FColor::Red, false, 5.0f);
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Orange, false, 5.0f, 0.0f, 2.0f);
	}
}


TWeakObjectPtr<USphereComponent> ATankBase::GetSphere() const
{
	return Sphere;
}

TWeakObjectPtr<USkeletalMeshComponent> ATankBase::GetBase() const
{
	return Base;
}

TWeakObjectPtr<USkeletalMeshComponent> ATankBase::GetTower() const
{
	return Tower;
}

TWeakObjectPtr<USpringArmComponent> ATankBase::GetSpringArm() const
{
	return SpringArmComponent;
}

TWeakObjectPtr<UCameraComponent> ATankBase::GetCamera() const
{
	return Camera;
}

TWeakObjectPtr<UTankMovementComponent> ATankBase::GetTankMovement() const
{
	return TankMovement;
}

EDrivingState ATankBase::GetCurrentDrivingState() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetCurrentDrivingState();
	}
	return EDrivingState::Idle;
}

FVector ATankBase::GetVelocityDirection() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetVelocityDirection();
	}
	else
	{
		return GetVelocity().GetSafeNormal();
	}
}

float ATankBase::GetCurrentSpeed() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetCurrentSpeed();
	}
	else
	{
		return GetVelocity().Size();
	}
}

float ATankBase::GetCurrentMaxSpeed() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetCurrentMaxSpeed();
	}
	return -1.0f;
}

float ATankBase::GetSlideDegree() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetSlideDegree();
	}
	return -1.0f;
}

bool ATankBase::GetIsSliding() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetIsSliding();
	}
	return false;
}

// FVector ATankBase::GetDesiredTargetRotation() const
// {
// 	return DesiredTowerAimLocation;
// }


