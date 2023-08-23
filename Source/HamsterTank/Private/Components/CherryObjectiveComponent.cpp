// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CherryObjectiveComponent.h"

// Sets default values for this component's properties
UCherryObjectiveComponent::UCherryObjectiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCherryObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCherryObjectiveComponent::OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup)
{
	if(Type != Cherry || !CollectedPickup.IsValid() || CollectedPickup->HasBeenCollected())
	{
		return;
	}
	CollectedPickup->SetCollected();
	CollectedCherries++;
	OnCherryCountChangedDelegateHandle.Broadcast(CollectedCherries);

	
}

float UCherryObjectiveComponent::GetCurrentCherries()
{
	return CollectedCherries;
}

