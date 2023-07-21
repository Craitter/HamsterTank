// Fill out your copyright notice in the Description page of Project Settings.



#include "TankBase.h"

#include "FireProjectileComponent.h"
#include "ProjectileOriginComponent.h"
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
	Body = CreateDefaultSubobject<USkeletalMeshComponent>("TankBase");
	if(!ensure(IsValid(Body))) return;
	Head = CreateDefaultSubobject<USkeletalMeshComponent>("TankHead");
	if(!ensure(IsValid(Head))) return;
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	if(!ensure(IsValid(SpringArmComponent))) return;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	if(!ensure(IsValid(Camera))) return;
	TankMovement = CreateDefaultSubobject<UTankMovementComponent>("TankMovement");
	if(!ensure(IsValid(TankMovement))) return;
	FireProjectileComponent = CreateDefaultSubobject<UFireProjectileComponent>("FireProjectileComponent");
	if(!ensure(IsValid(FireProjectileComponent))) return;
	ProjectileOriginComponent = CreateDefaultSubobject<UProjectileOriginComponent>("ProjectileOrigin");
	if(!ensure(IsValid(ProjectileOriginComponent))) return;
	
	SetRootComponent(Sphere);
	Body->SetupAttachment(Sphere);
	Head->SetupAttachment(Body, TEXT("BoneTurret"));
	Head->SetUsingAbsoluteRotation(true);
	// if(Tower->LeaderPoseComponent != Base)
	// {
	// 	
	// }
	SpringArmComponent->SetupAttachment(Head);
	SpringArmComponent->bDoCollisionTest = false;
	//Todo:Set more default values so we dont lose them when renaming for example
	Camera->SetupAttachment(SpringArmComponent);
	ProjectileOriginComponent->SetupAttachment(Head);

	
}



// Called when the game starts or when spawned
void ATankBase::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(Head))
	{
		Head->SetWorldRotation(FRotator(Head->GetComponentRotation().Pitch, GetActorRotation().Yaw, 0.0f));
	}
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

void ATankBase::RequestFire()
{
	if(IsValid(Head) && IsValid(Controller) && IsValid(FireProjectileComponent))
	{
		FireProjectileComponent->TryFireProjectile(this);
		
		const FVector StartLocation = Head->GetComponentLocation();
		constexpr float Distance =  1000.0f;
		const FVector EndLocation = StartLocation + Head->GetForwardVector() * Distance;
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
	return Body;
}

TWeakObjectPtr<USkeletalMeshComponent> ATankBase::GetTower() const
{
	return Head;
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

TWeakObjectPtr<UFireProjectileComponent> ATankBase::GetFireProjectileComponent() const
{
	return FireProjectileComponent;
}

EDrivingState ATankBase::GetCurrentDrivingState() const
{
	if(IsValid(TankMovement))
	{
		return TankMovement->GetCurrentDrivingState();
	}
	return EDrivingState::DS_Idle;
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

bool ATankBase::GetHasEndlessAmmo() const
{
	if(IsValid(FireProjectileComponent))
	{
		return FireProjectileComponent->GetHasEndlessAmmo();
	}
	return false;
}

int32 ATankBase::GetCurrentAmmo() const
{
	if(IsValid(FireProjectileComponent))
	{
		return FireProjectileComponent->GetCurrentAmmo();
	}
	return 0;
}

int32 ATankBase::GetMaxAmmo() const
{
	if(IsValid(FireProjectileComponent))
	{
		return FireProjectileComponent->GetMaxAmmo();
	}
	return 0;
}

FString ATankBase::GetBodyName()
{
	if(IsValid(Body))
	{
		UE_LOG(LogTemp, Warning , TEXT("%s"), *Body->GetReadableName());
		return Body->GetReadableName();
	}
	return FString();
}

// FVector ATankBase::GetDesiredTargetRotation() const
// {
// 	return DesiredTowerAimLocation;
// }


