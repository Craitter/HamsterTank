// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankBase.generated.h"

class UProjectileOriginComponent;
class UFireProjectileComponent;
enum class EDrivingState : uint8;
class AProjectileBase;
class USphereComponent;
class UFloatingPawnMovement;
class UTankMovementComponent;
class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;



UCLASS()
class HAMSTERTANK_API ATankBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATankBase();


	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// void UpdateTowerRotation(float DeltaTime) const;

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere = {nullptr};
	
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Body = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Head = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent = {nullptr};
	
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UTankMovementComponent> TankMovement = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UFireProjectileComponent> FireProjectileComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UProjectileOriginComponent> ProjectileOriginComponent = {nullptr};

	// UPROPERTY(EditDefaultsOnly, Category = "Tank|Aiming")
	// float MaxTowerTurningDegreePerSecond = 90.0f;

	
public:	
	// void RequestAimAtTarget(const FVector& TargetLocation);
	void RequestFire();
private:
	//InternValue
	// FVector DesiredTowerAimLocation = FVector::ZeroVector;
	
public: //simple Getter Functions
	TWeakObjectPtr<USphereComponent> GetSphere() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetBase() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetTower() const;

	TWeakObjectPtr<USpringArmComponent> GetSpringArm() const;

	TWeakObjectPtr<UCameraComponent> GetCamera() const;

	TWeakObjectPtr<UTankMovementComponent> GetTankMovement() const;

	TWeakObjectPtr<UFireProjectileComponent> GetFireProjectileComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	EDrivingState GetCurrentDrivingState() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	FVector GetVelocityDirection() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	float GetCurrentSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	float GetCurrentMaxSpeed() const;
	//returns -1 if we are not Sliding;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	float GetSlideDegree() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	bool GetIsSliding() const;
	
	
	UFUNCTION(BlueprintCallable, Category = "Tank|Firing")
	bool GetHasEndlessAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Firing")
	int32 GetCurrentAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Firing")
	int32 GetMaxAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Tank|Body")
	FString GetBodyName();

	// FVector GetDesiredTargetRotation() const;
};
