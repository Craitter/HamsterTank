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
	
}

TWeakObjectPtr<AController> UCherryObjectiveComponent::GetOwningController() const
{
	const TWeakObjectPtr<APawn> OwningPawn = Cast<APawn>(GetOwner());
	if(!OwningPawn.IsValid())
	{
		return nullptr;
	}
	return OwningPawn->GetController();
}



