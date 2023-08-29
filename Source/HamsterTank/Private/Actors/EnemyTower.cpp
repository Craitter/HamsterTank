// Fill out your copyright notice in the Description page of Project Settings.



#include "Actors/EnemyTower.h"


#include "HamsterTankGameState.h"
#include "ObjectiveSubsystem.h"
#include "Actors/PickupActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/FireProjectileComponent.h"
#include "Components/HandleDamageComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileOriginComponent.h"
#include "Components/TankMovementComponent.h"
#include "HamsterTank/HamsterTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"


#if ENABLE_DRAW_DEBUG && !NO_CVARS
static TAutoConsoleVariable<bool> CVarShowAllTowerTargeting(
	TEXT("Tower.Targeting.Debug.All"),
	false,
	TEXT("Shows all available Debug options for targeting by the Tower"),
	ECVF_Default);
#endif

// Sets default values
AEnemyTower::AEnemyTower()
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
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	if(!ensure(IsValid(HealthComponent))) return;
	HandleDamageComponent = CreateDefaultSubobject<UHandleDamageComponent>("HandleDamageComponent");
	if(!ensure(IsValid(HandleDamageComponent))) return;
	AnimSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>("AnimSkeleton");
	if(!ensure(IsValid(AnimSkeleton))) return;
	
	SetRootComponent(CapsuleCollider);
	Base->SetupAttachment(CapsuleCollider);
	Base->PrimaryComponentTick.bCanEverTick = false;
	Tower->SetupAttachment(Base);
	Tower->PrimaryComponentTick.bCanEverTick = false;
	ProjectileOrigin->SetupAttachment(Tower);
	AnimSkeleton->SetupAttachment(CapsuleCollider);
	AnimSkeleton->SetVisibility(false);
	AnimSkeleton->SetRelativeScale3D(FVector(1.23f));
	AnimSkeleton->PrimaryComponentTick.bCanEverTick = true;
	

	HealthComponent->SetMaxHealth(2.0f); //explain
	
	FFireProjectileData NewDefault;
	NewDefault.bApplyCooldown = true;
	NewDefault.bApplySpread = false;
	NewDefault.bApplyCustomSpeed = true;
	NewDefault.bRandomizeCooldown = true;
	NewDefault.bRandomizeSpeed = false;
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
void AEnemyTower::BeginPlay()
{
	Super::BeginPlay();

	
	if(GetWorld())
	{
		ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
	}
	
	if(TowerType != ETowerType::Custom)
	{
		bLoseTargetWhenOutOfFOV = false;
		bNeedsUnblockedVisionToAimTargetForFiring = false;
		bCanOnlyTargetLocationsInRotationRange = false;
		bRestrainFOVToRotationRange = false;
		bPredictRotatedLocation = false;
		TowerFireType = ETowerFireType::Max;
	}
	if(IsValid(AnimSkeleton))
	{
		AnimSkeleton->SetComponentTickEnabled(false);
	}
	OnPlayerFoundDelegateHandle.BindUObject(this, &ThisClass::OnPlayerFound);
	OnSinRotationFinishedDelegateHandle.BindUObject(this, &ThisClass::OnSinRotationFinished);

	const FRotator ActorRotation = GetActorRotation();
	RotationRangeUpperBound = FRotator(ActorRotation.Pitch, ActorRotation.Yaw + GetIdleRotationRange()/2, ActorRotation.Roll);
	RotationRangeLowerBound = FRotator(ActorRotation.Pitch, ActorRotation.Yaw - GetIdleRotationRange()/2, ActorRotation.Roll);
	if(IsValid(Tower) && IsValid(ProjectileOrigin))
	{
		SetSinStartEndRotation(Tower->GetComponentRotation(), RotationRangeLowerBound);

		InternOriginLocation = Tower->GetComponentLocation();
		InternOriginLocation.Z = ProjectileOrigin->GetComponentLocation().Z;
	}

	if(IsValid(HealthComponent))
	{
		HealthComponent->OnDeathDelegateHandle.AddUObject(this, &ThisClass::OnDeath); //unbind
	}
}

void AEnemyTower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(IsValid(HealthComponent))
	{
		HealthComponent->OnDeathDelegateHandle.RemoveAll(this);
	}
	OnSinRotationFinishedDelegateHandle.Unbind();
	OnPlayerFoundDelegateHandle.Unbind();
	Super::EndPlay(EndPlayReason);
}

void AEnemyTower::UpdateTargeting(const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(EnemyTower::UpdateTargeting);
	if(VerifyTargetValid())
	{
		float ProjectileSpeed;
		FVector AimTargetLocation;
		GetFireTargetLocation(TargetPawn->GetActorLocation(), AimTargetLocation, ProjectileSpeed);
		
#if ENABLE_DRAW_DEBUG && !NO_CVARS
		if(bDebug || CVarShowAllTowerTargeting->GetBool())
		{
			DrawDebugSphere(GetWorld(), AimTargetLocation, 30.0f, 10.0f, FColor::Orange);
		}
#endif
		
		if(IsAimTargetLocationValid(AimTargetLocation))
		{
			const FVector AimTargetDirection = AimTargetLocation - InternOriginLocation;
			if(RotateToDesiredRotationAtDegreeRate(AimTargetDirection.Rotation(), DeltaTime, MaxTurningDegreePerSecondTargeting))
			{

				FFireProjectileData Data;
				FireProjectileComponent->GetDefaultProjectileData(Data);
				Data.CustomSpeed = ProjectileSpeed;
				if(FireProjectileComponent->TryFireProjectile(nullptr, Data))
				{
#if ENABLE_DRAW_DEBUG && !NO_CVARS
					if(bDebug || CVarShowAllTowerTargeting->GetBool())
					{
						DrawDebugSphere(GetWorld(), AimTargetLocation, 30.0f, 10.0f, FColor::Red, false, GetDesiredProjectileTravelTime(AimTargetLocation) + 0.5f);
					}
#endif
				}
			}
		}
	}
	else
	{
		TargetPawn = nullptr;
		TowerTargetingState = ETowerTargetingState::TargetOutOfView;
		GetWorldTimerManager().SetTimer(WaitForTargetReappearHandle, this, &ThisClass::OnTargetLost, TimeBeforeRotatingBackToIdle);
	}
}

// Called every frame
void AEnemyTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TRACE_CPUPROFILER_EVENT_SCOPE(EnemyTower::Tick);
	if(ShouldSkipUpdate())
	{
		return;
	}

	switch (TowerTargetingState)
	{
		case ETowerTargetingState::Idle:
		case ETowerTargetingState::TargetLost:
			{
				const float InMaxAverageDegreePerSecond = TowerTargetingState == ETowerTargetingState::Idle ? AverageTurningDegreePerSecondIdle : AverageTurningDegreePerSecondBackToIdle;
				RotateTowerSin(InMaxAverageDegreePerSecond, DeltaTime);
			}
		case ETowerTargetingState::TargetOutOfView:
			LookForPlayer();	
			break;
		case ETowerTargetingState::Targeting:
			UpdateTargeting(DeltaTime);
		break;
	default: ;
	}
#if ENABLE_DRAW_DEBUG && !NO_CVARS
	DrawTickDebug();
#endif
	
}

bool AEnemyTower::IsAlive() const
{
	if(IsValid(HealthComponent))
	{
		return HealthComponent->IsAlive();
	}
	return false;
}

void AEnemyTower::OnDeath(TWeakObjectPtr<AController> DamageInstigator)
{
	if(ObjectiveSubsystem.IsValid())
	{
		ObjectiveSubsystem->TowerDestroyed(DamageInstigator.Get());
	}
	if(IsValid(AnimSkeleton) && IsValid(Tower) && IsValid(Base) && IsValid(CapsuleCollider))
	{
		AnimSkeleton->SetComponentTickEnabled(true);
		Tower->SetVisibility(false);
		Base->SetVisibility(false);
		AnimSkeleton->SetVisibility(true);
		AnimSkeleton->PlayAnimation(DeathAnimation, false);
		CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ThisClass::OnAnimFinshed, 3.0f, false);
		
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z -= CapsuleCollider->GetScaledCapsuleHalfHeight() - 20.0f;
		GetWorld()->SpawnActor<APickupActor>(APickupActor::StaticClass(), SpawnLocation, GetActorRotation());
	}
}

void AEnemyTower::OnAnimFinshed()
{
	if(IsValid(AnimSkeleton))
	{
		AnimSkeleton->SetComponentTickEnabled(false);
	}
}

void AEnemyTower::OnTargetLost()
{
	TowerTargetingState = ETowerTargetingState::TargetLost;
	CurrentTurningTime = 0.0f;

	const FRotator StartRotation = Tower->GetComponentRotation();
	const FQuat ActorQuaternion = GetActorQuat();
	const float DistanceToUpper = ActorQuaternion.AngularDistance(RotationRangeUpperBound.Quaternion());
	const float DistanceToLower = ActorQuaternion.AngularDistance(RotationRangeLowerBound.Quaternion());
	if(DistanceToLower > DistanceToUpper)
	{
		SetSinStartEndRotation(StartRotation, RotationRangeUpperBound);
	}
	else
	{
		SetSinStartEndRotation(StartRotation, RotationRangeLowerBound);
	}
}


void AEnemyTower::OnSinRotationFinished()
{	
	if(IsValid(Tower))
	{
		const FRotator StartRotation = Tower->GetComponentRotation();
		const FRotator EndRotation = StartRotation.Equals(RotationRangeUpperBound) ? RotationRangeLowerBound : RotationRangeUpperBound;
		
		SetSinStartEndRotation(StartRotation, EndRotation);
		
		if(TowerTargetingState == ETowerTargetingState::TargetLost)
		{
			TowerTargetingState = ETowerTargetingState::Idle;
		}
	}
}

void AEnemyTower::OnPlayerFound(const TWeakObjectPtr<APawn> InPlayerPawn)
{
	TargetPawn = InPlayerPawn;
	GetWorldTimerManager().ClearTimer(WaitForTargetReappearHandle);
	TowerTargetingState = ETowerTargetingState::Targeting;
}

void AEnemyTower::LookForPlayer()
{
	

	const TWeakObjectPtr<APawn> PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if(PlayerPawn.IsValid() && GetDistanceToSelf2D(PlayerPawn->GetActorLocation()) > GetMaxRange())
	{
		return;
	}
	TRACE_CPUPROFILER_EVENT_SCOPE(EnemyTower::LookForPlayer);
	const TWeakObjectPtr<UWorld> World = GetWorld();
	check(World.Get())

	TArray<FHitResult> Results;
	
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(GetMaxRange());

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.MobilityType = EQueryMobilityType::Dynamic;
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bIgnoreBlocks = bIgnoreBlocksWhenLookForPlayer;
	QueryParams.AddIgnoredActor(this);
	
	World->SweepMultiByProfile(Results, InternOriginLocation, InternOriginLocation, FQuat::Identity, LookForPlayerCollisionProfileName.Name, CollisionShape, QueryParams);

	for (const FHitResult& Result : Results)
	{
		const TWeakObjectPtr<APawn> HitPawn = Cast<APawn>(Result.GetActor());
		if(IsTargetAlive(HitPawn) && IsLocationInFieldOfView(HitPawn->GetActorLocation()) && CanTargetLocation(HitPawn->GetActorLocation(), HitPawn))
		{
			OnPlayerFoundDelegateHandle.Execute(HitPawn);
			return;
		}
	}
	TargetPawn = nullptr;
}

bool AEnemyTower::ShouldRestrainFOVToRotationRange() const
{
	return (bRestrainFOVToRotationRange || TowerType == ETowerType::OnTargetOnlyRotationRange);
}

bool AEnemyTower::IsLocationInFieldOfView(const FVector& TargetLocation) const
{
	const FVector NormalizedTargetDirection = (TargetLocation - InternOriginLocation).GetSafeNormal2D();
	bool bIsInFOV = IsDirectionInFOV(NormalizedTargetDirection);
	
	if(bIsInFOV && ShouldRestrainFOVToRotationRange())
	{
		bIsInFOV = IsDirectionInRotationRange(NormalizedTargetDirection);
	}
	return bIsInFOV;
}

bool AEnemyTower::CanTargetLocation(FVector TargetLocation, TWeakObjectPtr<APawn> PawnToIgnore) const
{
	TargetLocation.Z = InternOriginLocation.Z;
	FHitResult Result;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Result, InternOriginLocation, TargetLocation, ECC_GameTraceChannel2, QueryParams);
	
	return (!Result.bBlockingHit || Result.GetActor() == PawnToIgnore);
	
}

bool AEnemyTower::VerifyTargetValid() const
{
	if(IsTargetAlive() && IsLocationInRange(TargetPawn->GetActorLocation()) && CanTargetLocation(TargetPawn->GetActorLocation(), TargetPawn))
	{
		if(ShouldVerifyFieldOfView() && !IsLocationInFieldOfView(TargetPawn->GetActorLocation()))
		{
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool AEnemyTower::IsTargetAlive() const
{
	return IsTargetAlive(TargetPawn);
}

bool AEnemyTower::IsTargetAlive(TWeakObjectPtr<APawn> InTargetPawn)
{
	if(!InTargetPawn.IsValid())
	{
		return false;
	}
	const TWeakObjectPtr<UHealthComponent> PawnHealthComponent = InTargetPawn->FindComponentByClass<UHealthComponent>(); //Delegate
	return !PawnHealthComponent.IsValid() || PawnHealthComponent->IsAlive();
	
}

float AEnemyTower::GetMaxRange() const
{
	return MaxRange;
}

bool AEnemyTower::IsLocationInRange(const FVector& Location) const
{
	return GetDistanceToSelf2D(Location) < GetMaxRange();
}

bool AEnemyTower::ShouldVerifyFieldOfView() const
{
	return TowerType == ETowerType::OnTargetOnlyRotationRange || TowerType == ETowerType::OnTargetOnlyFOV || bLoseTargetWhenOutOfFOV;
}

void AEnemyTower::SetSinStartEndRotation(const FRotator& InStartRotation, const FRotator& InEndRotation)
{
	SinStartRotation = InStartRotation;
	SinEndRotation = InEndRotation;
	SinDeltaDegree = FMath::RadiansToDegrees(InStartRotation.Quaternion().AngularDistance(InEndRotation.Quaternion()));
	
}

float AEnemyTower::GetIdleRotationRange() const
{
	return IdleRotationRange;
}

bool AEnemyTower::IsDirectionInRotationRange(const FVector& Direction) const
{	
	const float DegreeDistance = FMath::RadiansToDegrees(acos(GetActorForwardVector().Dot(Direction)));
	return DegreeDistance < GetIdleRotationRange()/2;
	//Alternative
	// const FQuat TargetQuaternion = Direction.Rotation().Quaternion();
	// if(FMath::RadiansToDegrees(GetActorQuat().AngularDistance(TargetQuaternion)) > GetIdleRotationRange()/2)
	// {
	// 	return false;
	// }
}

float AEnemyTower::GetFOV() const
{
	return FOV;
}

bool AEnemyTower::IsDirectionInFOV(const FVector& Direction) const
{
	if(!IsValid(Tower))
	{
		return false;
	}
	const float DegreeDistance = FMath::RadiansToDegrees(acos(Tower->GetForwardVector().Dot(Direction)));
	return DegreeDistance < GetFOV()/2;
}

bool AEnemyTower::IsPredictingTower() const
{
	return TowerFireType == ETowerFireType::Predicted || TowerType == ETowerType::FullyPredicted || TowerType == ETowerType::LocationPredicted;
}

bool AEnemyTower::ShouldPredictRotation() const
{
	return TowerType == ETowerType::FullyPredicted || bPredictRotatedLocation;
}

bool AEnemyTower::ShouldVerifyTargetAimLocation() const
{
	return bNeedsUnblockedVisionToAimTargetForFiring || TowerType == ETowerType::OnTarget || TowerType == ETowerType::OnTargetOnlyRotationRange || TowerType == ETowerType::OnTargetOnlyFOV;
}

bool AEnemyTower::ShouldConstrainAimDirectionToRotationRange() const
{
	return bCanOnlyTargetLocationsInRotationRange || TowerType == ETowerType::OnTargetOnlyRotationRange;
}

bool AEnemyTower::ShouldSkipUpdate()
{
	const TWeakObjectPtr<AHamsterTankGameState> HamsterTankGameState = Cast<AHamsterTankGameState>(UGameplayStatics::GetGameState(this));
	if(HamsterTankGameState.IsValid() && HamsterTankGameState->HasMatchFinished())
	{
		return true;
	}
	return !IsValid(ProjectileOrigin) || !IsValid(Tower) || !IsValid(GetWorld()) || !IsAlive();
}

float AEnemyTower::GetDesiredProjectileTravelTime(const FVector& Location) const
{
	if(DistanceToTravelTimeCurve.GetRichCurveConst() != nullptr)
	{		
		return DistanceToTravelTimeCurve.GetRichCurveConst()->Eval(GetDistanceToSelf2D(Location));
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s() Curve not Setup in BP, fallback to 1 second"),*FString(__FUNCTION__));
		return 1.0f;
	}
	
}

void AEnemyTower::RotateTowerSin(const float AverageDegreePerSecond, const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(EnemyTower::RotateTowerSin);
	if(IsValid(Tower))
	{
		CurrentTurningTime += DeltaTime;

		const float TurningTime = SinDeltaDegree / AverageDegreePerSecond;
		const float Alpha = FMath::Clamp<float>(CurrentTurningTime / TurningTime, 0.0f, 1.0f);
		Tower->SetWorldRotation(FMath::InterpSinInOut(SinStartRotation, SinEndRotation, Alpha));
	
		if(Alpha + UE_FLOAT_NORMAL_THRESH > 1.0f && OnSinRotationFinishedDelegateHandle.IsBound())
		{
			CurrentTurningTime = 0.0f;
			OnSinRotationFinishedDelegateHandle.Execute();
		}
	}
}

void AEnemyTower::GetFireTargetLocation(const FVector& InTargetLocation,  FVector& OutFireTargetLocation, float& OutDesiredProjectileSpeed)
{
	if(!TargetPawn.IsValid() || !IsValid(Tower))
	{
		return;
	}
	const float DesiredTravelTime = GetDesiredProjectileTravelTime(InTargetLocation);
	
	if(IsPredictingTower())
	{
		const TWeakObjectPtr<UTankMovementComponent> TankMovementComponent = Cast<UTankMovementComponent>(TargetPawn->GetMovementComponent());
		if(TankMovementComponent.IsValid())
		{
			OutFireTargetLocation = TankMovementComponent->PredictLocationAfterSeconds(DesiredTravelTime, ShouldPredictRotation());
		}
	}
	else
	{
		OutFireTargetLocation = TargetPawn->GetActorLocation();
	}
	OutFireTargetLocation.Z = InternOriginLocation.Z;
	OutDesiredProjectileSpeed = GetDistanceToSelf2D(OutFireTargetLocation) / DesiredTravelTime;
}

bool AEnemyTower::RotateToDesiredRotationAtDegreeRate(const FRotator& DesiredRotation, const float DeltaTime, const float DesiredMaxDegreePerSecond) const
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

bool AEnemyTower::IsAimTargetLocationValid(const FVector& AimTargetLocation) const
{
	if(ShouldVerifyTargetAimLocation() && !CanTargetLocation(AimTargetLocation, TargetPawn))
	{
		return false;
	}
	if(ShouldConstrainAimDirectionToRotationRange())
	{
		const FVector DesiredAimDirection = (AimTargetLocation - InternOriginLocation).GetSafeNormal2D();
		if(!IsDirectionInRotationRange(DesiredAimDirection))
		{
			return false;
		}
	}
	return true;
}

float AEnemyTower::GetDistanceToSelf2D(const FVector& InLocation) const
{
	return FVector::Dist2D(InLocation, ProjectileOrigin->GetComponentLocation());
}

#if ENABLE_DRAW_DEBUG && !NO_CVARS
void AEnemyTower::DrawTickDebug() const
{
	if(bDebug || CVarShowAllTowerTargeting->GetBool())
	{
		DrawDebugCircleArc(GetWorld(), InternOriginLocation, GetMaxRange(), GetActorForwardVector(), FMath::DegreesToRadians(GetIdleRotationRange()/2), 20.0f, FColor::Yellow);
		DrawDebugLine(GetWorld(), InternOriginLocation, RotationRangeUpperBound.Vector() * GetMaxRange() + InternOriginLocation, FColor::Yellow);
		DrawDebugLine(GetWorld(), InternOriginLocation, RotationRangeLowerBound.Vector() * GetMaxRange() + InternOriginLocation, FColor::Yellow);

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
		DrawDebugCircleArc(GetWorld(), InternOriginLocation, GetMaxRange(), Tower->GetForwardVector(), FMath::DegreesToRadians(GetFOV()/2), 20.0f, DebugColor, false, -1.0f, 2.0f);
		const FVector StartDirection = FRotator(0.0f, GetFOV() / 2, 0.0f).RotateVector(Tower->GetForwardVector());
		for(int32 i = 0; i < GetFOV(); i++)
		{
			FVector NewDirection = FRotator(0.0f, -i, 0.0f).RotateVector(StartDirection);
			if(ShouldRestrainFOVToRotationRange())
			{
				if(FMath::RadiansToDegrees(NewDirection.Rotation().Quaternion().AngularDistance(GetActorForwardVector().Rotation().Quaternion())) > GetIdleRotationRange() / 2)
				{
					continue;
				}
			}
			FVector LineEnd = NewDirection * GetMaxRange() + InternOriginLocation;
			DrawDebugLine(GetWorld(), InternOriginLocation, LineEnd, DebugColor, false, -1.0f, 2.0f);
		}
	}
}
#endif