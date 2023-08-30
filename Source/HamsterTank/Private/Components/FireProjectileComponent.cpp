// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FireProjectileComponent.h"


#include "Components/ProjectileOriginComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile/ProjectileBase.h"
#include "Projectile/TankProjectileMovementComponent.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UFireProjectileComponent::UFireProjectileComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;
	bAutoActivate = true;

	bAutoFindProjectileOrigin = true;
	bHasEndlessAmmo = false;
	MaxAmmo = 10.0f;
	StartAmmo = 10.0f;
}

AProjectileBase* UFireProjectileComponent::TryFireProjectile(APawn* InstigatorPawn, const FFireProjectileData& FireData)
{
	if(!CanFireProjectile())
	{
		return nullptr;
	}
	
	FRotator SpawnRotation = ProjectileOrigin->GetComponentRotation();
	if(FireData.bApplySpread)
	{
		SpawnRotation.Yaw += FMath::FRandRange(-FireData.YawSpreadDegree, FireData.YawSpreadDegree);
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = InstigatorPawn;
	SpawnParameters.Owner = GetOwner();

	//Spawning Projectile
	const TWeakObjectPtr<AProjectileBase> NewProjectile = GetWorld()->SpawnActor<AProjectileBase>(BaseProjectile, ProjectileOrigin->GetComponentLocation(), SpawnRotation, SpawnParameters);
	
	if(FireData.bApplyCooldown)
	{
		const float OutCooldown = FireData.bRandomizeCooldown ? FMath::FRandRange(FireData.MinCooldown, FireData.MaxCooldown) : FireData.FireCooldown;
		GetOwner()->GetWorldTimerManager().SetTimer(FireCooldownTimerHandle, OutCooldown, false);
	}
	AddAmmoDelta(-1);
	
	if(NewProjectile.IsValid())
	{
		const TWeakObjectPtr<UTankProjectileMovementComponent> TankProjectileMovementComponent = NewProjectile->GetProjectileMovementComponent();
		if(TankProjectileMovementComponent.IsValid())
		{
			float OutSpeed = TankProjectileMovementComponent->Velocity.Size();
			if(FireData.bApplyCustomSpeed && FireData.CustomSpeed > 0.0f)
			{
				OutSpeed = FireData.CustomSpeed;
			}
			if(FireData.bRandomizeSpeed)
			{
				OutSpeed *= FMath::FRandRange(FireData.InMinSpeedModifier, FireData.InMaxSpeedModifier);
			}
			TankProjectileMovementComponent->SetVelocityInLocalSpace(FVector(OutSpeed, 0.0f, 0.0f));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s() Projectile couldnt be spawned"), *FString(__FUNCTION__));
	}
	return NewProjectile.Get();
}

AProjectileBase* UFireProjectileComponent::TryFireProjectile(APawn* InstigatorPawn)
{
	return TryFireProjectile(InstigatorPawn, DefaultFireProjectileData);
}

void UFireProjectileComponent::InitializeComponent()
{
	Super::InitializeComponent();
	if(bAutoFindProjectileOrigin)
	{
		SetProjectileOrigin();
	}
}

void UFireProjectileComponent::SetProjectileOrigin(const TWeakObjectPtr<USceneComponent> InProjectileOrigin)
{
	if(InProjectileOrigin.IsValid())
	{
		ProjectileOrigin = InProjectileOrigin;
	}
	else
	{
		const TWeakObjectPtr<AActor> Owner = GetOwner();
		if(Owner.IsValid())
		{
			ProjectileOrigin = Owner->FindComponentByClass<UProjectileOriginComponent>();
		}
	}
	if(!ProjectileOrigin.IsValid())
	{
		UE_LOG(LogTemp, Error , TEXT("There is no Projectile Origin, make sure to Add a UProjectileOriginComponent to your Actor or pass in a component with bAutoFindOrigin = false which serves as Origin... Falling Back to RootComponent"));
		const TWeakObjectPtr<AActor> Owner = GetOwner();
		if(Owner.IsValid())
		{
			ProjectileOrigin = Owner->GetRootComponent();
		}
	}
}


// Called when the game starts
void UFireProjectileComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = StartAmmo;
	OnAmmoChanged.Broadcast(CurrentAmmo);
	OnMaxAmmoChanged.AddUObject(this, &ThisClass::SetMaxAmmo);
	// ...
	
}

void UFireProjectileComponent::OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup)
{
	if(Type != Ammo || !CollectedPickup.IsValid() || CollectedPickup->HasBeenCollected() || CurrentAmmo >= MaxAmmo)
	{
		return;
	}
	CollectedPickup->SetCollected();
	AddAmmoDelta(Amount);
}

bool UFireProjectileComponent::GetHasEndlessAmmo() const
{
	return bHasEndlessAmmo;
}

int32 UFireProjectileComponent::GetMaxAmmo() const
{
	return MaxAmmo;
}

int32 UFireProjectileComponent::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

void UFireProjectileComponent::GetDefaultProjectileData(FFireProjectileData& FireProjectileData)
{
	FireProjectileData = DefaultFireProjectileData;
}

void UFireProjectileComponent::SetMaxAmmo(const int32 NewMaxAmmo, int32 InCurrentAmmo)
{
	MaxAmmo = NewMaxAmmo;
}

void UFireProjectileComponent::UpdateMaxAmmo(int32 NewMaxAmmo) const
{
	if(NewMaxAmmo < 0)
	{
		NewMaxAmmo = 0;
	}
	OnMaxAmmoChanged.Broadcast(NewMaxAmmo, CurrentAmmo);
}

void UFireProjectileComponent::SetStartAmmo(const int32 NewStartAmmo)
{
	StartAmmo = NewStartAmmo;
}

void UFireProjectileComponent::AddAmmoDelta(const int32 DeltaAmmo)
{
	if(!bHasEndlessAmmo)
	{
		CurrentAmmo = FMath::Clamp(CurrentAmmo + DeltaAmmo, 0, MaxAmmo);
		OnAmmoChanged.Broadcast(CurrentAmmo);	
	}

}

bool UFireProjectileComponent::CanFireProjectile() const
{

	if(!ProjectileOrigin.IsValid())
	{
		UE_LOG(LogTemp, Warning , TEXT("There is no Projectile Origin, @See SetProjectileOrigin()"));
		return false;
	}
	if(!IsValid(BaseProjectile))
	{
		UE_LOG(LogTemp, Warning , TEXT("There is no Projectile Class Reference set"));
		return false;
	}
	if(!IsValid(GetOwner()))
	{
		UE_LOG(LogTemp, Error , TEXT("The Owner is not Valid"));
		return false;
	}
	
	return (bHasEndlessAmmo || CurrentAmmo > 0) && !GetOwner()->GetWorldTimerManager().IsTimerActive(FireCooldownTimerHandle);
}

void UFireProjectileComponent::SetDefaultFireProjectileData(const FFireProjectileData& NewData)
{
	DefaultFireProjectileData = NewData;
}


