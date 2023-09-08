// Fill out your copyright notice in the Description page of Project Settings.



#include "Actors/PickupActor.h"


#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/TanksterGameplayEffect.h"

#include "Components/CapsuleComponent.h"
#include "HamsterTank/HamsterTank.h"

class UCollectPickupComponent;
// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>("CapsuleCollider");
	if(!ensure(IsValid(CapsuleCollider))) return;
	PickupParticle = CreateDefaultSubobject<UNiagaraComponent>("PickupParticle");
	if(!ensure(IsValid(PickupParticle))) return;

	SetRootComponent(CapsuleCollider);
	CapsuleCollider->SetCapsuleHalfHeight(12.0f);
	CapsuleCollider->SetCapsuleRadius(11.0f);
	CapsuleCollider->SetCollisionProfileName(FName("OverlapOnlyPawn"));
	PickupParticle->SetupAttachment(CapsuleCollider);
	
	const ConstructorHelpers::FObjectFinder<UPickupData> PDCommon(TEXT("/Game/Actors/Pickup/BackingData/PD_Common"));
	PickupMap.Add(EPickupRarity::Common, PDCommon.Object);

	const ConstructorHelpers::FObjectFinder<UPickupData> PDRare(TEXT("/Game/Actors/Pickup/BackingData/PD_Rare"));
	PickupMap.Add(EPickupRarity::Rare, PDRare.Object);

	const ConstructorHelpers::FObjectFinder<UPickupData> PDVeryRare(TEXT("/Game/Actors/Pickup/BackingData/PD_VeryRare"));
	PickupMap.Add(EPickupRarity::VeryRare, PDVeryRare.Object);

	const ConstructorHelpers::FObjectFinder<UPickupData> PDLegendary(TEXT("/Game/Actors/Pickup/BackingData/PD_Legendary"));
	PickupMap.Add(EPickupRarity::Legendary, PDLegendary.Object);
}

bool APickupActor::HasBeenCollected() const
{
	return bHasBeenCollected;
}

void APickupActor::SetCollected()
{
	bHasBeenCollected = true;
	if(IsValid(PickupParticle) && PickupData != nullptr)
	{
		PickupParticle->Deactivate();
		PickupParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupData->PickupNiagara, GetActorLocation(), GetActorRotation(), FVector(DEFAULT_PAWN_SCALE));
		PickupParticle->OnSystemFinished.AddDynamic(this, &ThisClass::OnNiagaraFinished);
	}
	// if(IsValid(CurrentPickupData.PickupSound))
	// {
	// 	UGameplayStatics::PlaySoundAtLocation(this, CurrentPickupData.PickupSound, GetActorLocation(), GetActorRotation());
	// }
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	InitializePickup();

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


void APickupActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(bHasBeenCollected)
	{
		return;
	}
	if(IsValid(OtherActor) && OtherActor != this)
	{
		if(PickupData != nullptr)
		{
			IAbilitySystemInterface* IAB = Cast<IAbilitySystemInterface>(OtherActor);
			if(IAB != nullptr)
			{
				const TWeakObjectPtr<UAbilitySystemComponent> TargetASC = Cast<UAbilitySystemComponent>(IAB);
				if(TargetASC.IsValid() && PickupData->PickupGameplayEffect != nullptr)
				{
					FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
					ContextHandle.AddInstigator(GetOwner(), this);
					const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectToSelf(PickupData->PickupGameplayEffect, 1.0f, ContextHandle);
					if(ActiveGameplayEffectHandle.WasSuccessfullyApplied())
					{
						SetCollected();
					}
				}
			}
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

void APickupActor::SetActivePickup(FPickupData_Backend* InPickupData)
{
	if(InPickupData != nullptr)
	{
		PickupParticle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, InPickupData->DefaultNiagara, GetActorLocation());
		PickupData = InPickupData;
	}
}

void APickupActor::InitializePickup()
{
	for (const auto& Data : PickupMap)
	{
		if(Data.Key == PickupRarity)
		{
			if(IsValid(Data.Value))
			{
				SetActivePickup(Data.Value->ChoosePickup());
			}
			break;
		}
	}
}

