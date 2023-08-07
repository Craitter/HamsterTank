// Fill out your copyright notice in the Description page of Project Settings.



#include "Actors/PickupActor.h"


#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Components/CapsuleComponent.h"
#include "Components/CollectPickupComponent.h"

class UCollectPickupComponent;
// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("CapsuleCollider");
	if(!ensure(IsValid(CapsuleCollider))) return;
	PickupParticle = CreateDefaultSubobject<UNiagaraComponent>("PickupParticle");
	if(!ensure(IsValid(PickupParticle))) return;

	SetRootComponent(CapsuleCollider);
	CapsuleCollider->SetCapsuleHalfHeight(80.0f);
	CapsuleCollider->SetCapsuleRadius(60.0f);
	CapsuleCollider->SetCollisionProfileName(FName("OverlapOnlyPawn"));
	PickupParticle->SetupAttachment(CapsuleCollider);
	
	const ConstructorHelpers::FObjectFinder<UDataTable> Pickup_Table(TEXT("/Game/Actors/Pickup/Pickups"));
	PickupTable = Pickup_Table.Object;
	if(!ensure(IsValid(PickupTable))) return;
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(PickupTable))
	{
		TArray<FPickupData*> Pickups;
		PickupTable->GetAllRows<FPickupData>(FString(), Pickups);


		const float RandomNumber = FMath::FRandRange(0.1f, 100.0f);
		for (const auto Pickup : Pickups)
		{
			if(Pickup != nullptr)
			{
				if(DefinedType == Pickup->Type)
				{
					PickupParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Pickup->DefaultNiagara, GetActorLocation());
					PickupEndParticle = Pickup->PickupNiagara;
					CurrentPickupData = *Pickup;
					break;
				}
				if(DefinedType == None && RandomNumber > Pickup->ProbabilityMin && RandomNumber < Pickup->ProbabilityMax)
				{
					PickupParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Pickup->DefaultNiagara, GetActorLocation());
					PickupEndParticle = Pickup->PickupNiagara;
					CurrentPickupData = *Pickup;
					break;
				}
			}
		}
	}
	if(CurrentPickupData.Type == Nothing)
	{
		Destroy();
	}

	if(IsValid(CapsuleCollider))
	{
		CapsuleCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	}
}

void APickupActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(IsValid(PickupParticle))
	{
		PickupParticle->OnSystemFinished.Clear();
	}
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsValid(OtherActor) && OtherActor != this)
	{
		const TWeakObjectPtr<UCollectPickupComponent> PickupComponent = OtherActor->FindComponentByClass<UCollectPickupComponent>();
		if(PickupComponent.IsValid())
		{
			PickupComponent->CollectPickup(CurrentPickupData);
			PickupParticle->Deactivate();
			PickupParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CurrentPickupData.PickupNiagara, GetActorLocation());
			PickupParticle->OnSystemFinished.AddDynamic(this, &ThisClass::OnNiagaraFinished);
		}
	}
}

void APickupActor::OnNiagaraFinished(UNiagaraComponent* FinishedSystem)
{
	if(FinishedSystem != nullptr)
	{
		FinishedSystem->Deactivate();
	}
	Destroy();
}

