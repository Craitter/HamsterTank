// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"


class USoundCue;
class UCapsuleComponent;
class UNiagaraSystem;
class UNiagaraComponent;


UENUM(BlueprintType)
enum EPickupType : uint8
{
	None = 0,

	Nothing = 1,
	
	Heal = 2,

	Ammo = 3,

	Cherry = 4,
};

USTRUCT(BlueprintType)
struct FPickupData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPickupType> Type = None;

	UPROPERTY(EditAnywhere)
	float Amount = 0.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> DefaultNiagara = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> PickupNiagara = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> PickupSound = {nullptr};
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100", ForceUnits = "Percentage"))
	float ProbabilityMin = 1.0f; 
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100", ForceUnits = "Percentage"))
	float ProbabilityMax = 1.0f; 
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
	TObjectPtr<UDataTable> PickupTable = {nullptr};

	UPROPERTY(EditInstanceOnly)
	TEnumAsByte<EPickupType> DefinedType = None;
public:
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnNiagaraFinished(UNiagaraComponent* FinishedSystem);
private:
	FPickupData CurrentPickupData;

	bool bHasBeenCollected = false;
	
};
