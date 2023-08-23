// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "Components/ActorComponent.h"
#include "CollectPickupComponent.generated.h"


class ICollectPickupInterface;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPickupCollectedDelegate, const EPickupType&, const float& /*Amount*/, TWeakObjectPtr<APickupActor>);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAMSTERTANK_API UCollectPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollectPickupComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	

	FOnPickupCollectedDelegate OnPickupCollectedDelegateHandle;
	void CollectPickup(const FPickupData& Data, TWeakObjectPtr<APickupActor> CollectedPickup);

private:
	TArray<ICollectPickupInterface*> BoundComponents;
};
