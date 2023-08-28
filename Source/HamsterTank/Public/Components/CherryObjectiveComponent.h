// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/CollectPickupInterface.h"
#include "CherryObjectiveComponent.generated.h"

class UObjectiveSubsystem;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCherryCountChangedDelegate, float)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAMSTERTANK_API UCherryObjectiveComponent : public UActorComponent, public ICollectPickupInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCherryObjectiveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TWeakObjectPtr<AController> GetOwningController() const;
	virtual void OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup) override;
public:
	

	
	float GetCurrentCherries() const;
private:
	
	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = {nullptr};
};
