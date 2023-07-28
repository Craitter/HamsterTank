// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UTankProjectileMovementComponent;
class UPointLightComponent;
class USphereComponent;

UCLASS()
class HAMSTERTANK_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> PointLight = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> ProjectileParticle = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTankProjectileMovementComponent> ProjectileMovement = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> FireEffect = {nullptr};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> HitEffect = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	bool bUseZeroRotationOnHit = false;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage = 1.0f;

	virtual float ComputeDamage();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<AActor*> Overlaps;
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION()
	void OnProjectileHit(const FHitResult& Result);

	UFUNCTION()
	void OnActorTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<UTankProjectileMovementComponent> GetProjectileMovementComponent();
};
