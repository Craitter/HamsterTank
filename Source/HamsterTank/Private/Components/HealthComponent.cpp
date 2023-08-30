// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentHealth = 6.0f;
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	OnHeathChangedDelegateHandle.Broadcast(CurrentHealth);
	// ...
	
}

void UHealthComponent::Heal(const float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0, MaxHealth);
	OnHeathChangedDelegateHandle.Broadcast(CurrentHealth);
}

void UHealthComponent::ReceiveFinalDamage(float FinalDamage)
{
	ReceiveFinalDamage(FinalDamage, nullptr);
}

void UHealthComponent::ReceiveFinalDamage(float FinalDamage, TWeakObjectPtr<AController> Instigator)
{
	if(IsImmortal())
	{
		return;
	}
	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0, MaxHealth);
	OnHeathChangedDelegateHandle.Broadcast(CurrentHealth);
	if(!IsAlive())
	{
		OnDeathDelegateHandle.Broadcast(Instigator);
	}
}


bool UHealthComponent::IsAlive() const
{
	return CurrentHealth > 0;
}

float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

void UHealthComponent::UpdateMaxHealth(float NewMaxHealth)
{
	OnMaxHealthChangedDelegateHandle.Broadcast(NewMaxHealth, MaxHealth, CurrentHealth);
	MaxHealth = NewMaxHealth;
}

bool UHealthComponent::IsImmortal() const
{
	return bIsImmortal;
}

void UHealthComponent::OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup)
{
	if(Type != EPickupType::Heal || !CollectedPickup.IsValid() || CollectedPickup->HasBeenCollected() || CurrentHealth + UE_FLOAT_NORMAL_THRESH >= MaxHealth)
	{
		return;
	}
	CollectedPickup->SetCollected();
	Heal(Amount);
}

