// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandleDamageComponent.generated.h"


enum EPickupType : uint8;
class UHealthComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAMSTERTANK_API UHandleDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHandleDamageComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	UFUNCTION()
	void OnOwnerTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName
	                       BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	bool bModifyDamageOnFront = false;
	float MultiplierFrontHit = 1.0f; 
	bool bModifyDamageOnRight = false;
	float MultiplierRightHit = 1.0f; 
	bool bModifyDamageOnLeft = false;
	float MultiplierLeftHit = 1.0f;
	bool bModifyDamageOnBack = false;
	float MultiplierBackHit = 1.0f; 

	FVector GetLastHitDirection();


protected:
	FVector LastHitDirection = FVector::ZeroVector;
private:
	TWeakObjectPtr<UHealthComponent> AssociatedHealthComponent = {nullptr};
};
