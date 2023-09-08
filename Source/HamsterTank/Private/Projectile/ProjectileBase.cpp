// Fill out your copyright notice in the Description page of Project Settings.



#include "Projectile/ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/TanksterGameplayEffect.h"
#include "Engine/DamageEvents.h"

#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "HamsterTank/HamsterTank.h"
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
	
	if(IsValid(Sphere))
	{
		// UE_LOG(LogTemp, Warning , TEXT("first %s"), *GetName());
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	}
	if(IsValid(ProjectileMovement))
	{
		ProjectileMovement->OnProjectileStop.AddDynamic(this, &ThisClass::OnProjectileHit);
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffect, GetActorLocation(), GetActorRotation(), FVector(DEFAULT_PAWN_SCALE), true, true, ENCPoolMethod::None);
}



void AProjectileBase::OnProjectileHit(const FHitResult& HitResult)
{
	if(!EffectToApply.IsValid())
	{
		bDestroyWhenReceivedEffect = true;
		CachedResult = HitResult;
		SetLifeSpan(0.1f);
		return;
	}
	
	const IAbilitySystemInterface* IAB = Cast<IAbilitySystemInterface>(HitResult.GetActor());
	if(IAB != nullptr && IAB->GetAbilitySystemComponent() != nullptr)
	{
		const TWeakObjectPtr<UAbilitySystemComponent> TargetASC = IAB->GetAbilitySystemComponent();
			
		EffectToApply.Data.Get()->GetContext().AddHitResult(HitResult);
		TargetASC->ApplyGameplayEffectSpecToTarget(*EffectToApply.Data, TargetASC.Get());
		Destroy();
	}
	else
	{
		if(IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsA(StaticClass()))
		{
			HitResult.GetActor()->Destroy();
		}
		Destroy();
	}
}

void AProjectileBase::Destroyed()
{
	// TWeakObjectPtr<UNiagaraComponent> RuntimeHitEffect =
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, GetActorLocation(), GetActorRotation(),
		FVector(DEFAULT_PAWN_SCALE), true, true, ENCPoolMethod::None);
	if(IsValid(PointLight))
	{
		PointLight->SetIntensity(0.0f);
	}
	Super::Destroyed();
}

void AProjectileBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if(IsValid(OtherActor) && OtherActor != this && OtherActor != GetOwner())
	{
		const FHitResult Result = FHitResult(OtherActor, nullptr, GetActorLocation(), GetActorForwardVector() * -1);

		OnProjectileHit(Result);
	}
}

void AProjectileBase::SetEffectToApplyToTarget(FGameplayEffectSpecHandle SpecHandle)
{
	EffectToApply = SpecHandle;
	if(bDestroyWhenReceivedEffect)
	{
		OnProjectileHit(CachedResult);
	}
}

TWeakObjectPtr<UTankProjectileMovementComponent> AProjectileBase::GetProjectileMovementComponent() const
{
	return ProjectileMovement;
}

