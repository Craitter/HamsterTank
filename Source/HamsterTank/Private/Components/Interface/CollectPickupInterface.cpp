// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Interface/CollectPickupInterface.h"

// Add default functionality here for any ICollectPickupInterface functions that are not pure virtual.
void ICollectPickupInterface::ListenToCollectComponent(FOnPickupCollectedDelegate& OnPickupCollectedDelegate)
{
	OnPickupCollectedDelegate.AddRaw(this, &ThisClass::OnPickupCollected);
}
