// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataCollections/PickupData.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"


enum EPickupType : uint8;
class UPickupData;
class USoundCue;
class UCapsuleComponent;
class UNiagaraSystem;
class UNiagaraComponent;


UENUM()
enum class EPickupRarity
{
	Common,

	Rare,

	VeryRare,

	Legendary,
};

UCLASS()
class HAMSTERTANK_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

	bool HasBeenCollected() const;

	void SetCollected();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleCollider = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> PickupParticle = {nullptr};
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> PickupEndParticle = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EPickupType> Type = None;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<EPickupRarity, TObjectPtr<UPickupData>> PickupMap;

	EPickupRarity PickupRarity = EPickupRarity::Common;

	FPickupData_Backend* PickupData = {nullptr};
public:
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnNiagaraFinished(UNiagaraComponent* FinishedSystem);
private:
	void SetActivePickup(FPickupData_Backend* InPickupData);
	
	void InitializePickup();
	bool bHasBeenCollected = false;
	
};
