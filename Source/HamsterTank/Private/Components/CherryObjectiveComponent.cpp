// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CherryObjectiveComponent.h"

// Sets default values for this component's properties
UCherryObjectiveComponent::UCherryObjectiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCherryObjectiveComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCherryObjectiveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCherryObjectiveComponent::OnPickupCollected(const EPickupType& Type, const float& Amount)
{
	if(Type != Cherry)
	{
		return;
	}
	CollectedCherries++;
	OnCherryCountChangedDelegateHandle.Broadcast(CollectedCherries);

	
}

float UCherryObjectiveComponent::GetCurrentCherries()
{
	return CollectedCherries;
}

