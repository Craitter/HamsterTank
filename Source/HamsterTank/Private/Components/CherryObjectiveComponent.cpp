// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CherryObjectiveComponent.h"

#include "ObjectiveSubsystem.h"

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

	if(GetOwner() && GetOwner()->GetGameInstance())
	{
		ObjectiveSubsystem = GetOwner()->GetGameInstance()->GetSubsystem<UObjectiveSubsystem>();
	}
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

void UCherryObjectiveComponent::OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup)
{
	if(Type != Cherry || !CollectedPickup.IsValid() || CollectedPickup->HasBeenCollected())
	{
		return;
	}
	CollectedPickup->SetCollected();
	if(ObjectiveSubsystem.IsValid())
	{
		ObjectiveSubsystem->CherryCollected(GetOwningController(), FMath::RoundToInt32(Amount));
	}	
}

float UCherryObjectiveComponent::GetCurrentCherries() const
{
	if(!ObjectiveSubsystem.IsValid())
	{
		return 0;
	}
	const FObjectiveScore* Score = ObjectiveSubsystem->GetScore(GetOwningController());
	if(Score == nullptr)
	{
		return 0;
	}
	return Score->GetCherries();
}

