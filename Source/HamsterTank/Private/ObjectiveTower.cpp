// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveTower.h"

#include "FireProjectileComponent.h"
#include "ProjectileOriginComponent.h"
#include "TankMovementComponent.h"
#include "Components/CapsuleComponent.h"



#if ENABLE_DRAW_DEBUG && !NO_CVARS
static TAutoConsoleVariable<bool> CVarShowAllTowerTargeting(
	TEXT("Tower.Targeting.Debug.All"),
	false,
	TEXT("Shows all available Debug options for targeting by the Tower"),
	ECVF_Default);
#endif

// Sets default values
AObjectiveTower::AObjectiveTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("CapsuleCollider");
	if(!ensure(IsValid(CapsuleCollider))) return;
	Base = CreateDefaultSubobject<USkeletalMeshComponent>("Base");
	if(!ensure(IsValid(Base))) return;
	Tower = CreateDefaultSubobject<USkeletalMeshComponent>("Tower");
	if(!ensure(IsValid(Tower))) return;
	FireProjectileComponent = CreateDefaultSubobject<UFireProjectileComponent>("FireProjectileComponent");
	if(!ensure(IsValid(FireProjectileComponent))) return;
	ProjectileOrigin = CreateDefaultSubobject<UProjectileOriginComponent>("ProjectileOrigin");
	if(!ensure(IsValid(ProjectileOrigin))) return;
	
	SetRootComponent(CapsuleCollider);
	Base->SetupAttachment(CapsuleCollider);
	Tower->SetupAttachment(Base);
	ProjectileOrigin->SetupAttachment(Tower);

	FFireProjectileData NewDefault;
	NewDefault.bApplyCooldown = true;
	NewDefault.bApplySpread = false;
	NewDefault.bRandomizeCooldown = true;
	NewDefault.bRandomizeSpeed = false;
	NewDefault.bApplyCustomSpeed = true;
	NewDefault.FireCooldown = 1.0f;
	NewDefault.MinCooldown = 0.9f;
	NewDefault.MaxCooldown = 1.8f;
	NewDefault.YawSpreadDegree = 2.0f;
	NewDefault.InMaxSpeedModifier = 0.95f;
	NewDefault.InMinSpeedModifier = 1.05f;
	FireProjectileComponent->SetDefaultFireProjectileData(NewDefault);
	
	CurrentTurningTime = 0.0f;

	TowerFireType = ETowerFireType::OnTarget;

	bIgnoreBlocksWhenLookForPlayer = true;

	//Todo: more constructor values, debugger, comment everything, check that the height wont get a problem at all!!! Fix Snapping at the beginning and when back at idle 
	//Fire projectile component same
	//Then test it all out with debugging and everything set
	//Then implement fire modes
}

// Called when the game starts or when spawned
void AObjectiveTower::BeginPlay()
{
	Super::BeginPlay();

	if(TowerType != ETowerType::Custom)
	{
		bLoseTargetWhenOutOfFOV = false;
		bNeedsUnblockedVisionToAimTargetForFiring = false;
		bCanOnlyTargetLocationsInRotationRange = false;
		bRestrainFOVToRotationRange = false;
		bPredictRotatedLocation = false;
	}
	
	OnPlayerFoundDelegateHandle.BindUObject(this, &ThisClass::OnPlayerFound);
	OnSinRotationFinishedDelegateHandle.BindUObject(this, &ThisClass::OnSinRotationFinished);

	const FRotator ActorRotation = GetActorRotation();
	RotationRangeUpperBound = FRotator(ActorRotation.Pitch, ActorRotation.Yaw + IdleRotationRange/2, ActorRotation.Roll);
	RotationRangeLowerBound = FRotator(ActorRotation.Pitch, ActorRotation.Yaw - IdleRotationRange/2, ActorRotation.Roll);
	if(IsValid(Tower) && IsValid(ProjectileOrigin))
	{
		SinStartRotation = Tower->GetComponentRotation();
		SinEndRotation = RotationRangeLowerBound;

		InternTargetingOriginLocation = Tower->GetComponentLocation();
		InternTargetingOriginLocation.Z = ProjectileOrigin->GetComponentLocation().Z;
	}
}

void AObjectiveTower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnSinRotationFinishedDelegateHandle.Unbind();
	OnPlayerFoundDelegateHandle.Unbind();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AObjectiveTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(ShouldSkipUpdate())
	{
		return;
	}

#if ENABLE_DRAW_DEBUG && !NO_CVARS
	if(bDebug || CVarShowAllTowerTargeting->GetBool())
	{
		DrawDebugCircleArc(GetWorld(), InternTargetingOriginLocation, MaxTrackDistance, GetActorForwardVector(), FMath::DegreesToRadians(IdleRotationRange/2), 20.0f, FColor::Yellow);
		DrawDebugLine(GetWorld(), InternTargetingOriginLocation, RotationRangeUpperBound.Vector() * MaxTrackDistance + InternTargetingOriginLocation, FColor::Yellow);
		DrawDebugLine(GetWorld(), InternTargetingOriginLocation, RotationRangeLowerBound.Vector() * MaxTrackDistance + InternTargetingOriginLocation, FColor::Yellow);
	}
#endif
	
	if(TowerTargetingState != ETowerTargetingState::Targeting)
	{
		if (TowerTargetingState == ETowerTargetingState::TargetLost || TowerTargetingState == ETowerTargetingState::Idle)
		{
			const float InMaxAverageDegreePerSecond = TowerTargetingState == ETowerTargetingState::Idle ? AverageTurningDegreePerSecondIdle : AverageTurningDegreePerSecondBackToIdle;
			RotateTowerSin(InMaxAverageDegreePerSecond, DeltaTime);
		}
		LookForPlayer(InternTargetingOriginLocation);	
	}
	else
	{
		if(IsStillTargeting())
		{
			SetDesiredTravelTime(GetDistanceToSelf2D(PlayerPawn->GetActorLocation()));
			
			const FVector AimTargetLocation = GetFireTargetLocation();
			
#if ENABLE_DRAW_DEBUG && !NO_CVARS
			if(bDebug || CVarShowAllTowerTargeting->GetBool())
			{
				DrawDebugSphere(GetWorld(), AimTargetLocation, 30.0f, 10.0f, FColor::Orange);
			}
#endif
			
			if(IsAimTargetLocationValid(AimTargetLocation))
			{
				const FVector AimTargetDirection = AimTargetLocation - InternTargetingOriginLocation;
				if(RotateToDesiredRotationAtDegreeRate(AimTargetDirection.Rotation(), DeltaTime, MaxTurningDegreePerSecondTargeting))
				{
					const float ProjectileSpeed = GetDistanceToSelf2D(AimTargetLocation) / InternDesiredProjectileTravelTime;

					FFireProjectileData Data;
					FireProjectileComponent->GetDefaultProjectileData(Data);
					Data.CustomSpeed = ProjectileSpeed;
					FireProjectileComponent->SetDefaultFireProjectileData(Data);
					if(FireProjectileComponent->TryFireProjectile(nullptr))
					{
#if ENABLE_DRAW_DEBUG && !NO_CVARS
						if(bDebug || CVarShowAllTowerTargeting->GetBool())
						{
							DrawDebugSphere(GetWorld(), AimTargetLocation, 30.0f, 10.0f, FColor::Red, false, InternDesiredProjectileTravelTime + 0.5f);
						}
#endif
					}
				}
			}
		}
		else
		{
			TowerTargetingState = ETowerTargetingState::TargetOutOfView;
			GetWorldTimerManager().SetTimer(WaitForTargetReappearHandle, this, &ThisClass::OnTargetLost, TimeBeforeRotatingBackToIdle);
		}
	}

#if ENABLE_DRAW_DEBUG && !NO_CVARS
	if(bDebug || CVarShowAllTowerTargeting->GetBool())
	{
		FColor DebugColor;
		switch (TowerTargetingState)
		{
		case ETowerTargetingState::Idle:
				DebugColor = FColor::Green;
				break;
		case ETowerTargetingState::Targeting:
				DebugColor = FColor::Red;
				break;
		case ETowerTargetingState::TargetOutOfView:
				DebugColor = FColor::Orange;
				break;
		case ETowerTargetingState::TargetLost:
				DebugColor = FColor::Magenta;
				break;
			default: ;
		}
		DrawDebugCircleArc(GetWorld(), InternTargetingOriginLocation, MaxTrackDistance, Tower->GetForwardVector(), FMath::DegreesToRadians(FOV/2), 20.0f, DebugColor, false, -1.0f, 2.0f);
		const FVector StartDirection = FRotator(0.0f, FOV / 2, 0.0f).RotateVector(Tower->GetForwardVector());
		for(int32 i = 0; i < FOV; i++)
		{
			FVector NewDirection = FRotator(0.0f, -i, 0.0f).RotateVector(StartDirection);
			if(bRestrainFOVToRotationRange || TowerType == ETowerType::OnTargetOnlyRotationRange)
			{
				if(FMath::RadiansToDegrees(NewDirection.Rotation().Quaternion().AngularDistance(GetActorForwardVector().Rotation().Quaternion())) > IdleRotationRange / 2)
				{
					continue;
				}
			}
			FVector LineEnd = NewDirection * MaxTrackDistance + InternTargetingOriginLocation;
			DrawDebugLine(GetWorld(), InternTargetingOriginLocation, LineEnd, DebugColor, false, -1.0f, 2.0f);
		}
	}
#endif
}

void AObjectiveTower::OnTargetLost()
{
	TowerTargetingState = ETowerTargetingState::TargetLost;
	CurrentTurningTime = 0.0f;

	SinStartRotation = Tower->GetComponentRotation();
	const FQuat ActorQuaternion = GetActorQuat();
	const float DistanceToUpper = ActorQuaternion.AngularDistance(RotationRangeUpperBound.Quaternion());
	const float DistanceToLower = ActorQuaternion.AngularDistance(RotationRangeLowerBound.Quaternion());
	if(DistanceToLower > DistanceToUpper)
	{
		SinEndRotation = RotationRangeUpperBound;
	}
	else
	{
		SinEndRotation = RotationRangeLowerBound;
	}
}


void AObjectiveTower::OnSinRotationFinished()
{	
	if(IsValid(Tower))
	{
		SinStartRotation = Tower->GetComponentRotation();
		SinEndRotation = SinStartRotation.Equals(RotationRangeUpperBound) ? RotationRangeLowerBound : RotationRangeUpperBound;
		
		if(TowerTargetingState == ETowerTargetingState::TargetLost)
		{
			TowerTargetingState = ETowerTargetingState::Idle;
		}
	}
}

void AObjectiveTower::OnPlayerFound(const TWeakObjectPtr<APawn> InPlayerPawn)
{
	PlayerPawn = InPlayerPawn;
	GetWorldTimerManager().ClearTimer(WaitForTargetReappearHandle);
	TowerTargetingState = ETowerTargetingState::Targeting;
}

void AObjectiveTower::LookForPlayer(const FVector& StartLocation)
{
	const TWeakObjectPtr<UWorld> World = GetWorld();
	check(World.Get())

	TArray<FHitResult> Results;
	const FVector EndLocation = StartLocation;
	
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(MaxTrackDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.MobilityType = EQueryMobilityType::Dynamic;
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bIgnoreBlocks = bIgnoreBlocksWhenLookForPlayer;
	QueryParams.AddIgnoredActor(this);
	
	World->SweepMultiByProfile(Results, StartLocation, EndLocation, FQuat::Identity, LookForPlayerCollisionProfileName.Name, CollisionShape, QueryParams);

	for (const FHitResult& Result : Results)
	{
		TWeakObjectPtr<AActor> HitActor = Result.GetActor();
		if(HitActor.IsValid() && HitActor->IsA(APawn::StaticClass()))
		{
			TWeakObjectPtr<APawn> HitPawn = Cast<APawn>(HitActor);
			if(HitPawn.IsValid() && HitPawn->IsPlayerControlled())
			{
				FVector AdjustedPawnLocation = HitPawn->GetActorLocation();
				AdjustedPawnLocation.Z = InternTargetingOriginLocation.Z;
				if(IsLocationInFieldOfView(StartLocation, AdjustedPawnLocation))
				{
					FHitResult InResult;
					TryTargetLocation(StartLocation, AdjustedPawnLocation, InResult);
					if(!InResult.bBlockingHit || InResult.GetActor() == HitPawn)
					{
						OnPlayerFoundDelegateHandle.Execute(HitPawn);
						return;
					}
				}
			}
		}
	}
	PlayerPawn = nullptr;
}

bool AObjectiveTower::IsLocationInFieldOfView(const FVector& StartLocation, const FVector& TargetLocation) const
{
	const FVector NormalizedTargetDirection = (TargetLocation - StartLocation).GetSafeNormal();
	
	float DegreeDistance = FMath::RadiansToDegrees(acos(Tower->GetForwardVector().Dot(NormalizedTargetDirection)));
	bool bIsInFOV = DegreeDistance < FOV/2;

	if(bIsInFOV && bRestrainFOVToRotationRange)
	{
		DegreeDistance = FMath::RadiansToDegrees(acos(GetActorForwardVector().Dot(NormalizedTargetDirection)));
		bIsInFOV = DegreeDistance < IdleRotationRange/2;
	}
	return bIsInFOV;
}

void AObjectiveTower::TryTargetLocation(const FVector& StartLocation, const FVector& TargetLocation, FHitResult& OutHitResult) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(OutHitResult, StartLocation, TargetLocation, ECC_GameTraceChannel2, QueryParams);
}

bool AObjectiveTower::IsStillTargeting() const
{
	if(PlayerPawn.IsValid())
	{
		FVector AdjustedPawnLocation = PlayerPawn->GetActorLocation();
		AdjustedPawnLocation.Z = InternTargetingOriginLocation.Z;
		
		if(GetDistanceToSelf2D(AdjustedPawnLocation) > MaxTrackDistance)
		{
			return false;
		}
		FHitResult Result;
		TryTargetLocation(InternTargetingOriginLocation, AdjustedPawnLocation, Result);
		if(Result.bBlockingHit && Result.GetActor() != PlayerPawn)
		{
			return false;
		}
		if((TowerType == ETowerType::OnTargetOnlyRotationRange || TowerType == ETowerType::OnTargetOnlyFOV || bLoseTargetWhenOutOfFOV) && !IsLocationInFieldOfView(InternTargetingOriginLocation,AdjustedPawnLocation))
		{
			return false;
		}
		return true;
	}
	return false;
}

bool AObjectiveTower::ShouldSkipUpdate()
{
	return !IsValid(ProjectileOrigin) && !IsValid(Tower) && !IsValid(GetWorld());
}

void AObjectiveTower::SetDesiredTravelTime(float InDistance)
{
	if(DistanceToTravelTimeCurve.GetRichCurveConst() != nullptr)
	{		
		InternDesiredProjectileTravelTime = DistanceToTravelTimeCurve.GetRichCurveConst()->Eval(InDistance);
		return;
	}
	InternDesiredProjectileTravelTime = 1.0f;
}

void AObjectiveTower::RotateTowerSin(const float AverageDegreePerSecond, const float DeltaTime)
{
	if(IsValid(Tower))
	{
		CurrentTurningTime += DeltaTime;
		
		const float DeltaDegree = FMath::RadiansToDegrees(SinStartRotation.Quaternion().AngularDistance(SinEndRotation.Quaternion()));
		const float TurningTime = DeltaDegree / AverageDegreePerSecond;
		const float Alpha = FMath::Clamp<float>(CurrentTurningTime / TurningTime, 0.0f, 1.0f);
		Tower->SetWorldRotation(FMath::InterpSinInOut(SinStartRotation, SinEndRotation, Alpha));
	
		if(Alpha + UE_FLOAT_NORMAL_THRESH > 1.0f && OnSinRotationFinishedDelegateHandle.IsBound())
		{
			CurrentTurningTime = 0.0f;
			OnSinRotationFinishedDelegateHandle.Execute();
		}
	}
}

FVector AObjectiveTower::GetFireTargetLocation()
{
	if(!PlayerPawn.IsValid() || !IsValid(Tower))
	{
		return FVector::ZeroVector; 
	}
	FVector OutTargetLocation = FVector::ZeroVector;
	
	const FVector PlayerLocation = PlayerPawn->GetActorLocation();
	
	if(TowerFireType == ETowerFireType::Predicted || TowerType == ETowerType::FullyPredicted || TowerType == ETowerType::LocationPredicted)
	{
		const TWeakObjectPtr<UTankMovementComponent> TankMovementComponent = Cast<UTankMovementComponent>(PlayerPawn->GetMovementComponent());
		if(TankMovementComponent.IsValid())
		{
			const bool bOutPredictRotation = (TowerType == ETowerType::FullyPredicted || bPredictRotatedLocation);
			OutTargetLocation = TankMovementComponent->PredictLocationAfterSeconds(InternDesiredProjectileTravelTime, bOutPredictRotation);
		}
	}
	else
	{
		OutTargetLocation = PlayerLocation;
	}
	OutTargetLocation.Z = InternTargetingOriginLocation.Z;
	return OutTargetLocation;
}

bool AObjectiveTower::RotateToDesiredRotationAtDegreeRate(const FRotator& DesiredRotation, const float DeltaTime, const float DesiredMaxDegreePerSecond) const
{
	if(IsValid(Tower))
	{
		const FQuat TargetQuaternion = DesiredRotation.Quaternion();
		const FQuat ShortestRotation = FQuat::FindBetweenVectors(Tower->GetForwardVector(), DesiredRotation.Vector());
		const float DeltaDegree = FMath::RadiansToDegrees(ShortestRotation.GetAngle());
		const float AngleRatio = FMath::Clamp( (DeltaTime * DesiredMaxDegreePerSecond) / DeltaDegree, 0.0f, 1.0f);
		const FQuat NewRotation = FQuat::Slerp(Tower->GetComponentRotation().Quaternion(),TargetQuaternion , AngleRatio);
				
		Tower->SetWorldRotation(NewRotation);
		if(TargetQuaternion.Equals(NewRotation))
		{
			return true;
		}
	}
	return false;
}

bool AObjectiveTower::IsAimTargetLocationValid(const FVector& AimTargetLocation) const
{
	if(IsValid(Tower))
	{
		if(bNeedsUnblockedVisionToAimTargetForFiring || TowerType == ETowerType::OnTarget || TowerType == ETowerType::OnTargetOnlyRotationRange || TowerType == ETowerType::OnTargetOnlyFOV)
		{
			FHitResult Result;
			TryTargetLocation(InternTargetingOriginLocation, AimTargetLocation, Result);
			if(Result.bBlockingHit && Result.GetActor() != PlayerPawn)
			{
				return false;
			}
		}
		if(bCanOnlyTargetLocationsInRotationRange || TowerType == ETowerType::OnTargetOnlyRotationRange)
		{
			const FVector AimTargetDirection = AimTargetLocation - InternTargetingOriginLocation;
			const FQuat TargetQuaternion = AimTargetDirection.Rotation().Quaternion();
			if(FMath::RadiansToDegrees(GetActorQuat().AngularDistance(TargetQuaternion)) > IdleRotationRange/2)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

float AObjectiveTower::GetDistanceToSelf2D(const FVector& InLocation) const
{
	return FVector::Dist2D(InLocation, ProjectileOrigin->GetComponentLocation());
}