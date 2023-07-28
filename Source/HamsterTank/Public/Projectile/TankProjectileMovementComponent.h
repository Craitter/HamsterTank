// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "TankProjectileMovementComponent.generated.h"

/** This is mostly copied from @see UProjectileMovementComponent, this allows to cut unnecessary things and add custom functionality easily 
 * 
 */
UCLASS()
class HAMSTERTANK_API UTankProjectileMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnProjectileStopDelegate, const FHitResult&, ImpactResult );
public:
	UTankProjectileMovementComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	

	//Begin UMovementComponent Interface
	virtual void InitializeComponent() override;
	virtual void UpdateTickRegistration() override;
	//End UMovementComponent Interface



	
	/** Called when projectile has come to a stop (velocity is below simulation threshold, bounces are disabled, or it is forcibly stopped). */
	UPROPERTY(BlueprintAssignable)
	FOnProjectileStopDelegate OnProjectileStop;
protected:

	virtual bool HandleBlockingHit(const FHitResult& Hit, float DeltaTime, const FVector& MoveDelta);
	
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	
	/** Compute the distance we should move in the given time, at a given a velocity. */
	virtual FVector ComputeMoveDelta(const FVector& InVelocity, float DeltaTime) const;


	/** Compute the acceleration that will be applied */
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const;

	
	/**
	 * Given an initial velocity and a time step, compute a new velocity.
	 * Default implementation applies the result of ComputeAcceleration() to velocity.
	 * 
	 * @param  InitialVelocity Initial velocity.
	 * @param  DeltaTime Time step of the update.
	 * @return Velocity after DeltaTime time step.
	 */
	virtual FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const;

	
	/** Don't allow velocity magnitude to exceed MaxSpeed, if MaxSpeed is non-zero. */
	UFUNCTION(BlueprintCallable, Category="Game|Components|ProjectileMovement")
	FVector LimitVelocity(FVector NewVelocity) const;

	/**
 * This will check to see if the projectile is still in the world.  It will check things like
 * the KillZ, outside world bounds, etc. and handle the situation.
 */
	virtual bool CheckStillInWorld();

	/** Clears the reference to UpdatedComponent, fires stop event (OnProjectileStop), and stops ticking (if bAutoUpdateTickRegistration is true). */
	UFUNCTION(BlueprintCallable, Category="Game|Components|ProjectileMovement")
	virtual void StopSimulating(const FHitResult& HitResult);

	bool HasStoppedSimulation();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	uint8 bRotationFollowsVelocity:1;

	// Double-buffer of pending force so that updates can use the accumulated value and reset the data so other AddForce() calls work correctly.
	// Also prevents accumulation over frames where the update aborts for whatever reason, and works with substepping movement.
	FVector PendingForceThisUpdate;

	/** Initial speed of projectile. If greater than zero, this will override the initial Velocity value and instead treat Velocity as a direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float InitialSpeed;
	
	/**
	 * If true, the initial Velocity is interpreted as being in local space upon startup.
	 * @see SetVelocityInLocalSpace()
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	uint8 bInitialVelocityInLocalSpace:1;
	
	/** Limit on speed of projectile (0 means no limit). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Projectile)
	float MaxSpeed;

private:
	
	// Pending force for next tick.
	FVector PendingForce;

public:
	
	/** Sets the velocity to the new value, rotated into Actor space. */
	UFUNCTION(BlueprintCallable, Category="Game|Components|ProjectileMovement")
	virtual void SetVelocityInLocalSpace(FVector NewVelocity);
	
	/** Adds a force which is accumulated until next tick, used by ComputeAcceleration() to affect Velocity. */
	void AddForce(FVector Force);
	
	/** Clears any pending forces from AddForce(). If bClearImmediateForce is true, clears any force being processed during this update as well. */
	void ClearPendingForce(bool bClearImmediateForce = false);
	
	/** Returns the sum of pending forces from AddForce(). */
	FVector GetPendingForce() const;

	virtual float GetMaxSpeed() const override;
};


