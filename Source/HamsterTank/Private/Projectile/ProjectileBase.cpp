// Fill out your copyright notice in the Description page of Project Settings.



#include "Projectile/ProjectileBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Projectile/TankProjectileMovementComponent.h"


// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	if(!ensure(IsValid(Sphere))) return;
	PointLight = CreateDefaultSubobject<UPointLightComponent>("PointLight");
	if(!ensure(IsValid(PointLight))) return;
	ProjectileParticle = CreateDefaultSubobject<UNiagaraComponent>("Particle");
	if(!ensure(IsValid(ProjectileParticle))) return;
	ProjectileMovement = CreateDefaultSubobject<UTankProjectileMovementComponent>("ProjectileMovement");
	if(!ensure(IsValid(ProjectileMovement))) return;

	SetRootComponent(Sphere);
	PointLight->SetupAttachment(Sphere);
	ProjectileParticle->SetupAttachment(Sphere);
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnActorTakeAnyDamage);
	if(IsValid(Sphere))
	{
		// UE_LOG(LogTemp, Warning , TEXT("first %s"), *GetName());
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	}
	if(IsValid(ProjectileMovement))
	{
		ProjectileMovement->OnProjectileStop.AddDynamic(this, &ThisClass::OnProjectileHit);
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffect, GetActorLocation(), GetActorRotation(), FVector(1), true, true, ENCPoolMethod::None);
}

float AProjectileBase::ComputeDamage()
{
	return BaseDamage;
}

void AProjectileBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(IsValid(OtherActor) && OtherActor != this && OtherActor != GetOwner())
	{
		FHitResult Result = SweepResult;
		if(SweepResult.GetActor() == this)
		{
			Result = FHitResult(OtherActor, nullptr, GetActorLocation(), GetActorForwardVector() * -1);
		}		
		OnProjectileHit(Result);
	}
}


void AProjectileBase::OnProjectileHit(const FHitResult& Result)
{
	if(Result.GetActor() == this)
	{
		return;
	}
//Visual Changes
	FRotator HitEffectRotation = FRotator::ZeroRotator;
	if(!bUseZeroRotationOnHit)
	{
		HitEffectRotation = FRotationMatrix::MakeFromX(Result.ImpactNormal).Rotator();
	}
	FVector Location = Result.Location.IsNearlyZero() ? GetActorLocation() : Result.ImpactPoint;

	// TWeakObjectPtr<UNiagaraComponent> RuntimeHitEffect =
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Location, HitEffectRotation,
		FVector(1.0f), true, true, ENCPoolMethod::None);
	if(IsValid(PointLight))
	{
		PointLight->SetIntensity(0.0f);
	}

	FPointDamageEvent PointDamageEvent;
	PointDamageEvent.HitInfo = Result;
	PointDamageEvent.Damage = BaseDamage;
	PointDamageEvent.ShotDirection = GetActorForwardVector();
	
	//Apply "Damage"
	if(IsValid(Result.GetActor()))
	{
		Result.GetActor()->TakeDamage(ComputeDamage(), PointDamageEvent, GetInstigatorController(), GetInstigator());
	}
	Destroy();
}

void AProjectileBase::OnActorTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	bUseZeroRotationOnHit = true;
	FHitResult Result;
	Result.Location = FVector::ZeroVector;
	OnProjectileHit(Result);
}

TWeakObjectPtr<UTankProjectileMovementComponent> AProjectileBase::GetProjectileMovementComponent()
{
	return ProjectileMovement;
}

