// Fill out your copyright notice in the Description page of Project Settings.



#include "Actors/EnemyTower.h"


#include "AbilitySystem/TanksterAbilitySet.h"
#include "AbilitySystem/TanksterAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AmmoAttributeSet.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"
#include "Actors/PickupActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/FireProjectileComponent.h"
#include "Components/HandleDamageComponent.h"
#include "Components/HealthComponent.h"
#include "Components/ProjectileOriginComponent.h"
#include "Components/TankMovementComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameClasses/HamsterTankGameState.h"
#include "HamsterTank/HamsterTank.h"
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
	TowerBase = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	if(!ensure(IsValid(TowerBase))) return;
	TowerHead = CreateDefaultSubobject<UStaticMeshComponent>("Head");
	if(!ensure(IsValid(TowerHead))) return;
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
	TanksterAbilitySystem = CreateDefaultSubobject<UTanksterAbilitySystemComponent>("AbilitySystem");
	if(!ensure(IsValid(TanksterAbilitySystem))) return;
	HealthAttributeSet = CreateDefaultSubobject<UHealthAttributeSet>("HealthAttributeSet");
	if(!ensure(IsValid(HealthAttributeSet))) return;
	AmmoAttributeSet = CreateDefaultSubobject<UAmmoAttributeSet>("AmmoAttributeSet");
	if(!ensure(IsValid(AmmoAttributeSet))) return;
	
	SetRootComponent(CapsuleCollider);
	TowerBase->SetupAttachment(CapsuleCollider);
	TowerBase->PrimaryComponentTick.bCanEverTick = false;
	TowerHead->SetupAttachment(TowerBase);
	TowerHead->PrimaryComponentTick.bCanEverTick = false;
	ProjectileOrigin->SetupAttachment(TowerHead);
	AnimSkeleton->SetupAttachment(CapsuleCollider);
	AnimSkeleton->SetVisibility(false);
	AnimSkeleton->SetRelativeScale3D(FVector(DEFAULT_PAWN_SCALE));
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

	TanksterAbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Full);
}

// Called when the game starts or when spawned
void AEnemyTower::BeginPlay()
{
	Super::BeginPlay();
	
	IdleRotationRangeRadians = FMath::DegreesToRadians(IdleRotationRange);
	FOVRadians = FMath::DegreesToRadians(FOV);

	
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
	if(IsValid(TowerHead) && IsValid(ProjectileOrigin))
	{
		SetSinStartEndRotation(TowerHead->GetComponentRotation(), RotationRangeLowerBound);

		InternOriginLocation = TowerHead->GetComponentLocation();
		InternOriginLocation.Z = ProjectileOrigin->GetComponentLocation().Z;
	}

	if(IsValid(HealthComponent))
	{
		HealthComponent->OnDeathDelegateHandle.AddUObject(this, &ThisClass::OnDeath); //unbind
	}

	if(IsValid(TanksterAbilitySystem))
	{
		TanksterAbilitySystem->InitAbilityActorInfo(this, this);

		if(IsValid(HealthAttributeSet))
		{
			//Begin Binding Delegates On CharacterAttributeValueChanged
			HealthChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
			MaxHealthChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
		}
		if(IsValid(AmmoAttributeSet))
		{
			//Begin Binding Delegates On AmmoAttributeValueChanged
			AmmoChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetAmmoAttribute()).AddUObject(this, &ThisClass::AmmoChanged);
			MaxAmmoChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetMaxAmmoAttribute()).AddUObject(this, &ThisClass::MaxAmmoChanged);
			//End Binding Delegates On AmmoAttributeValueChanged
		}
		if(IsValid(InitialAbilitySet))
		{
			InitialAbilitySet->GiveToAbilitySystem(TanksterAbilitySystem, &GrantedHandles, this);
		}
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

	if(IsValid(TanksterAbilitySystem))
	{

		if(IsValid(HealthAttributeSet))
		{
			//Begin Remove Binding Delegates On HealthAttributeValueChanged
			TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
			TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetMaxHealthAttribute()).Remove(MaxHealthChangedDelegateHandle);
			//End Remove Binding Delegates On HealthAttributeValueChanged
		}
		
		if(IsValid(AmmoAttributeSet))
		{
			//Begin Remove Binding Delegates On AmmoAttributeValueChanged
			TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetAmmoAttribute()).Remove(AmmoChangedDelegateHandle);
			TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetMaxAmmoAttribute()).Remove(MaxAmmoChangedDelegateHandle);
			//End Remove Binding Delegates On AmmoAttributeValueChanged
		}
		
	}
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	AmmoChangedDelegateHandle.Reset();
	MaxAmmoChangedDelegateHandle.Reset();
	
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
	if(IsValid(AnimSkeleton) && IsValid(TowerHead) && IsValid(TowerBase) && IsValid(CapsuleCollider))
	{
		TowerHead->SetVisibility(false);
		TowerBase->SetVisibility(false);
		AnimSkeleton->SetComponentTickEnabled(true);
		AnimSkeleton->SetVisibility(true);
		AnimSkeleton->PlayAnimation(DeathAnimation, false);
		CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ThisClass::OnAnimFinished, 3.0f, false);
		
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z -= CapsuleCollider->GetScaledCapsuleHalfHeight() - 20.0f;
		GetWorld()->SpawnActor<APickupActor>(APickupActor::StaticClass(), SpawnLocation, GetActorRotation());
	}
}

void AEnemyTower::OnAnimFinished()
{
	if(IsValid(AnimSkeleton))
	{
		AnimSkeleton->SetComponentTickEnabled(false);
		if(IsValid(DeadMesh))
		{
			const TWeakObjectPtr<AStaticMeshActor> DeadTower = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), AnimSkeleton->GetComponentTransform());
			if(DeadTower.IsValid() && IsValid(DeadTower->GetStaticMeshComponent()))
			{
				DeadTower->SetMobility(EComponentMobility::Stationary);
				DeadTower->GetStaticMeshComponent()->SetStaticMesh(DeadMesh);
			}
			Destroy();
		}
	}
	
}

void AEnemyTower::OnTargetLost()
{
	TowerTargetingState = ETowerTargetingState::TargetLost;
	CurrentTurningTime = 0.0f;

	const FRotator StartRotation = TowerHead->GetComponentRotation();
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
	if(IsValid(TowerHead))
	{
		const FRotator StartRotation = TowerHead->GetComponentRotation();
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
	//If Multiplayer revisit
	if(IsTargetAlive(PlayerPawn) && IsLocationInFieldOfView(PlayerPawn->GetActorLocation()) && CanTargetLocation(PlayerPawn->GetActorLocation(), PlayerPawn))
	{
		OnPlayerFoundDelegateHandle.Execute(PlayerPawn);
		return;
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
		return !ShouldVerifyFieldOfView() || IsLocationInFieldOfView(TargetPawn->GetActorLocation());
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
	const float RadiansDistance = acos(GetActorForwardVector().Dot(Direction));
	return RadiansDistance < IdleRotationRangeRadians;
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
	if(!IsValid(TowerHead))
	{
		return false;
	}
	//Todo: Make Radians to improve performance
	const float RadiansDistance = acos(TowerHead->GetForwardVector().Dot(Direction));
	return RadiansDistance < FOVRadians;
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
	return !IsValid(ProjectileOrigin) || !IsValid(TowerHead) || !IsValid(GetWorld()) || !IsAlive();
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
	if(IsValid(TowerHead))
	{
		CurrentTurningTime += DeltaTime;

		const float TurningTime = SinDeltaDegree / AverageDegreePerSecond;
		const float Alpha = FMath::Clamp<float>(CurrentTurningTime / TurningTime, 0.0f, 1.0f);
		TowerHead->SetWorldRotation(FMath::InterpSinInOut(SinStartRotation, SinEndRotation, Alpha));
	
		if(Alpha + UE_FLOAT_NORMAL_THRESH > 1.0f)
		{
			CurrentTurningTime = 0.0f;
			OnSinRotationFinishedDelegateHandle.ExecuteIfBound();
		}
	}
}

void AEnemyTower::GetFireTargetLocation(const FVector& InTargetLocation,  FVector& OutFireTargetLocation, float& OutDesiredProjectileSpeed)
{
	if(!TargetPawn.IsValid() || !IsValid(TowerHead))
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
	if(IsValid(TowerHead))
	{
		const FQuat TargetQuaternion = DesiredRotation.Quaternion();
		const FQuat ShortestRotation = FQuat::FindBetweenVectors(TowerHead->GetForwardVector(), DesiredRotation.Vector());
		const float DeltaDegree = FMath::RadiansToDegrees(ShortestRotation.GetAngle());
		const float AngleRatio = FMath::Clamp( (DeltaTime * DesiredMaxDegreePerSecond) / DeltaDegree, 0.0f, 1.0f);
		const FQuat NewRotation = FQuat::Slerp(TowerHead->GetComponentRotation().Quaternion(),TargetQuaternion , AngleRatio);
				
		TowerHead->SetWorldRotation(NewRotation);
		if(TargetQuaternion.Equals(NewRotation))
		{
			return true;
		}
	}
	return false;
}

void AEnemyTower::HealthChanged(const FOnAttributeChangeData& Data)
{
}

void AEnemyTower::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
}

void AEnemyTower::AmmoChanged(const FOnAttributeChangeData& Data)
{
}

void AEnemyTower::MaxAmmoChanged(const FOnAttributeChangeData& Data)
{
}

float AEnemyTower::GetHealth() const
{
	if(IsValid(HealthAttributeSet))
	{
		return HealthAttributeSet->GetHealth();
	}
	return 0.0f;
}

float AEnemyTower::GetMaxHealth() const
{
	if(IsValid(HealthAttributeSet))
	{
		return HealthAttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

float AEnemyTower::GetAmmo() const
{
	if(IsValid(AmmoAttributeSet))
	{
		return AmmoAttributeSet->GetAmmo();
	}
	return 0.0f;
}

float AEnemyTower::GetMaxAmmo() const
{
	if(IsValid(AmmoAttributeSet))
	{
		return AmmoAttributeSet->GetMaxAmmo();
	}
	return 0.0f;
}

UAbilitySystemComponent* AEnemyTower::GetAbilitySystemComponent() const
{
	return TanksterAbilitySystem;
}

TWeakObjectPtr<UHealthAttributeSet> AEnemyTower::GetHealthAttributeSet() const
{
	return HealthAttributeSet;
}

TWeakObjectPtr<UAmmoAttributeSet> AEnemyTower::GetAmmoAttributeSet() const
{
	return AmmoAttributeSet;
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
		DrawDebugCircleArc(GetWorld(), InternOriginLocation, GetMaxRange(), TowerHead->GetForwardVector(), FMath::DegreesToRadians(GetFOV()/2), 20.0f, DebugColor, false, -1.0f, 2.0f);
		const FVector StartDirection = FRotator(0.0f, GetFOV() / 2, 0.0f).RotateVector(TowerHead->GetForwardVector());
		for(int32 i = 0; i < GetFOV(); i++)
		{
			FVector NewDirection = FRotator(0.0f, -i, 0.0f).RotateVector(StartDirection);
			if(ShouldRestrainFOVToRotationRange())
			{
				if(NewDirection.Rotation().Quaternion().AngularDistance(GetActorForwardVector().Rotation().Quaternion()) > IdleRotationRangeRadians)
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