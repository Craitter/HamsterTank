// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PickupActor.h"
#include "Components/ActorComponent.h"
#include "CollectPickupComponent.generated.h"


class ICollectPickupInterface;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPickupCollectedDelegate, const EPickupType&, const float& /*Amount*/);

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnPickupCollectedDelegate OnPickupCollectedDelegateHandle;
	void CollectPickup(const FPickupData& Data);

private:
	TArray<ICollectPickupInterface*> BoundComponents;
};
