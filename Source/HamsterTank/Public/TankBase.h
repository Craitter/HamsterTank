// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankBase.generated.h"

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

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere = {nullptr};
	
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Base = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Tower = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArm = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UTankMovementComponent> TankMovement = {nullptr};
	

public:	

private:
	
public: //simple Getter Functions
	TWeakObjectPtr<USphereComponent> GetBox() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetBase() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetTower() const;

	TWeakObjectPtr<USpringArmComponent> GetSpringArm() const;

	TWeakObjectPtr<UCameraComponent> GetCamera() const;

	TWeakObjectPtr<UTankMovementComponent> GetTankMovement() const;
	
};
