// Fill out your copyright notice in the Description page of Project Settings.




#include "Actors/TankBase.h"

#include "AbilitySystem/TanksterAbilitySet.h"
#include "AbilitySystem/TanksterAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AmmoAttributeSet.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "AbilitySystem/GameplayAbilities/PredictMovementAbility.h"
#include "Actors/EnemyTower.h"
#include "Camera/CameraComponent.h"
#include "Components/CherryObjectiveComponent.h"
#include "Components/HandleDamageComponent.h"
#include "Components/ProjectileOriginComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TankMovementComponent.h"
#include "GameClasses/Player/TanksterPlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "HamsterTank/HamsterTank.h"


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
	ProjectileOriginComponent = CreateDefaultSubobject<UProjectileOriginComponent>("ProjectileOrigin");
	if(!ensure(IsValid(ProjectileOriginComponent))) return;
	HandleDamageComponent = CreateDefaultSubobject<UHandleDamageComponent>("HandleDamageComponent");
	if(!ensure(IsValid(HandleDamageComponent))) return;
	CherryObjectiveComponent = CreateDefaultSubobject<UCherryObjectiveComponent>("CherryObjectiveComponent");
	if(!ensure(IsValid(CherryObjectiveComponent))) return;
	
	SetRootComponent(Sphere);
	Body->SetupAttachment(Sphere);
	Body->SetRelativeScale3D(FVector(DEFAULT_PAWN_SCALE));
	Head->SetupAttachment(Body, TEXT("BoneTurret"));
	Head->SetUsingAbsoluteRotation(true);
	SpringArmComponent->SetupAttachment(Sphere);
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritRoll = false;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = false;
	// Todo:Set more default values so we dont lose them when renaming for example
	Camera->SetupAttachment(SpringArmComponent);
	ProjectileOriginComponent->SetupAttachment(Head);

	HandleDamageComponent->bModifyDamageOnRight = true;
	HandleDamageComponent->MultiplierRightHit = 2.0f;
	HandleDamageComponent->bModifyDamageOnLeft = true;
	HandleDamageComponent->MultiplierLeftHit = 2.0f;


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

void ATankBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
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
	if(IsValid(Head))
	{
		Head->SetRelativeRotation(FRotator(Head->GetRelativeRotation().Pitch, SpringArmComponent->GetTargetRotation().Yaw, Head->GetRelativeRotation().Roll));
	}
}

void ATankBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	const TObjectPtr<ATanksterPlayerState> PS = GetPlayerState<ATanksterPlayerState>();
	if(IsValid(PS))
	{
		HealthAttributeSet = PS->GetHealthAttributeSet();
		AmmoAttributeSet = PS->GetAmmoAttributeSet();
		
		TanksterAbilitySystem = Cast<UTanksterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	}
	if(TanksterAbilitySystem.IsValid())
	{
		TanksterAbilitySystem->InitAbilityActorInfo(PS, this);
		if(IsValid(InitialAbilitySet))
		{
			InitialAbilitySet->GiveToAbilitySystem(TanksterAbilitySystem.Get(), &GrantedHandles, this);
		}
	}
}

void ATankBase::ClearVelocity() const
{
	TankMovement->Velocity = FVector::ZeroVector;
}


// void ATankBase::RequestAimAtTarget(const FVector& TargetLocation)
// {
// 	if(IsValid(Tower) && !bIsTargetLocationLocked)
// 	{
// 		DesiredTowerAimLocation = TargetLocation;
// 		DesiredTowerAimLocation.Z = Tower->GetComponentLocation().Z;
// 	}
// }


void ATankBase::OnActorDied(TWeakObjectPtr<AController> DamageInstigator)
{
	ClearVelocity();
}

void ATankBase::SetMovementPredictionAbility(const TWeakObjectPtr<UPredictMovementAbility> InAbility, bool bRotated)
{
	if(InAbility.IsValid())
	{
		if(bRotated)
		{
			PredictRotatedLocationAbility = InAbility;
		}
		else
		{
			PredictLocationAbility = InAbility;
		}
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

FVector ATankBase::GetPredictedLocation(const float& Time, const bool bRotated) const
{
	if(PredictLocationAbility.IsValid() && PredictRotatedLocationAbility.IsValid())
	{
		if(bRotated)
		{
			return PredictRotatedLocationAbility->GetPredictedLocationForTime(Time);
		}
		else
		{
			return PredictLocationAbility->GetPredictedLocationForTime(Time);
		}
	}
	return GetActorLocation();
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

FString ATankBase::GetBodyName() const
{
	if(IsValid(Body))
	{
		return Body->GetReadableName();
	}
	return FString();
}

bool ATankBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

FVector ATankBase::GetLastHitDirection() const
{
	if(IsValid(HandleDamageComponent))
	{
		return HandleDamageComponent->GetLastHitDirection();
	}
	return FVector::ZeroVector;
}

TWeakObjectPtr<UTanksterAbilitySystemComponent> ATankBase::GetTanksterAbilitySystemComponent() const
{
	return TanksterAbilitySystem;
}

UAbilitySystemComponent* ATankBase::GetAbilitySystemComponent() const
{
	return TanksterAbilitySystem.Get();
}

float ATankBase::GetHealth() const
{
	if(HealthAttributeSet.IsValid())
	{
		return HealthAttributeSet->GetHealth();
	}
	return 0.0f;
}

float ATankBase::GetMaxHealth() const
{
	if(HealthAttributeSet.IsValid())
	{
		return HealthAttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

float ATankBase::GetAmmo() const
{
	if(AmmoAttributeSet.IsValid())
	{
		return AmmoAttributeSet->GetAmmo();
	}
	return 0.0f;
}
float ATankBase::GetMaxAmmo() const
{
	if(AmmoAttributeSet.IsValid())
	{
		return AmmoAttributeSet->GetMaxAmmo();
	}
	return 0.0f;
}

// FVector ATankBase::GetDesiredTargetRotation() const
// {
// 	return DesiredTowerAimLocation;
// }


