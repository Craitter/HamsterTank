// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CollectPickupComponent.h"
#include "UObject/Interface.h"
#include "CollectPickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCollectPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HAMSTERTANK_API ICollectPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void ListenToCollectComponent(FOnPickupCollectedDelegate& OnPickupCollectedDelegate);
	UFUNCTION()
	virtual void OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup) = 0;
};
