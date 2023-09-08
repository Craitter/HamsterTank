// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"


class UTanksterGameplayEffect;
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


	virtual void Destroyed() override;

	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetEffectToApplyToTarget(FGameplayEffectSpecHandle SpecHandle);
	
	TWeakObjectPtr<UTankProjectileMovementComponent> GetProjectileMovementComponent() const;
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

private:
	UFUNCTION()
	virtual void OnProjectileHit(const FHitResult& HitResult);

	FGameplayEffectSpecHandle EffectToApply;
	bool bDestroyWhenReceivedEffect = false;
	FHitResult CachedResult;

	
	

	
};
