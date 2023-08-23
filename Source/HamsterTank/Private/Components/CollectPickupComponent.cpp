// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CollectPickupComponent.h"

#include "Actors/PickupActor.h"
#include "Components/Interface/CollectPickupInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UCollectPickupComponent::UCollectPickupComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCollectPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	const TWeakObjectPtr<AActor> Owner = GetOwner();
	if(Owner.IsValid())
	{
		TArray<UActorComponent*> Components;
		Owner->GetComponents<UActorComponent>(Components);
		for (TWeakObjectPtr<UActorComponent> Component : Components)
		{
			if(Component.IsValid() && Component->Implements<UCollectPickupInterface>())
			{
				ICollectPickupInterface* CollectPickupInterface = Cast<ICollectPickupInterface>(Component);
				if(CollectPickupInterface != nullptr)
				{
					CollectPickupInterface->ListenToCollectComponent(OnPickupCollectedDelegateHandle);
					BoundComponents.Add(CollectPickupInterface);
				}
			}
		}
	}
	// ...
	
}

void UCollectPickupComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnPickupCollectedDelegateHandle.Clear();
	Super::EndPlay(EndPlayReason);
}



void UCollectPickupComponent::CollectPickup(const FPickupData& Data, TWeakObjectPtr<APickupActor> CollectedPickup)
{
	for (const auto Component : BoundComponents)
	{
		if(Component == nullptr && OnPickupCollectedDelegateHandle.IsBoundToObject(Component))
		{
			UE_LOG(LogTemp, Warning , TEXT("%s() A Raw Delegate was null"), *FString(__FUNCTION__));
			OnPickupCollectedDelegateHandle.RemoveAll(Component);
		}
	}
	OnPickupCollectedDelegateHandle.Broadcast(Data.Type, Data.Amount, CollectedPickup);
}

