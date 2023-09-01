// Fill out your copyright notice in the Description page of Project Settings.


#include "GameClasses/Player/TanksterPlayerState.h"

#include "AbilitySystem/TanksterAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/AmmoAttributeSet.h"
#include "AbilitySystem/AttributeSets/HealthAttributeSet.h"

ATanksterPlayerState::ATanksterPlayerState()
{
	TanksterAbilitySystem = CreateDefaultSubobject<UTanksterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	if(!ensure(TanksterAbilitySystem != nullptr)) return;
	HealthAttributeSet = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthAttributeSet"));
	if(!ensure(HealthAttributeSet != nullptr)) return;
	AmmoAttributeSet = CreateDefaultSubobject<UAmmoAttributeSet>(TEXT("AmmoAttributeSet"));
	if(!ensure(AmmoAttributeSet != nullptr)) return;

	
	TanksterAbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	
}

void ATanksterPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(TanksterAbilitySystem))
	{
		if(IsValid(HealthAttributeSet))
		{
			//Begin Binding Delegates On HealthAttributeValueChanged
			HealthChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
			MaxHealthChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
			//End Binding Delegates On HealthAttributeValueChanged
		}
		if(IsValid(AmmoAttributeSet))
		{
			//Begin Binding Delegates On AmmoAttributeValueChanged
			AmmoChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetAmmoAttribute()).AddUObject(this, &ThisClass::AmmoChanged);
			MaxAmmoChangedDelegateHandle = TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetMaxAmmoAttribute()).AddUObject(this, &ThisClass::MaxAmmoChanged);
			//End Binding Delegates On AmmoAttributeValueChanged
		}
	}
}

void ATanksterPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(IsValid(TanksterAbilitySystem))
	{

		if(IsValid(HealthAttributeSet))
		{
			//Begin Remove Binding Delegates On HealthAttributeValueChanged
			TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
			TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetMaxHealthAttribute()).Remove(MaxHealthChangedDelegateHandle);
			//End Remove Binding Delegates On HealthAttributeValueChanged
		}
		
		if(IsValid(AmmoAttributeSet))
		{
			//Begin Remove Binding Delegates On AmmoAttributeValueChanged
			 TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetAmmoAttribute()).Remove(AmmoChangedDelegateHandle);
			 TanksterAbilitySystem->GetGameplayAttributeValueChangeDelegate(AmmoAttributeSet->GetMaxAmmoAttribute()).Remove(MaxAmmoChangedDelegateHandle);
			//End Remove Binding Delegates On AmmoAttributeValueChanged
		}
		
	}
	HealthChangedDelegateHandle.Reset();
	MaxHealthChangedDelegateHandle.Reset();
	AmmoChangedDelegateHandle.Reset();
	MaxAmmoChangedDelegateHandle.Reset();
	
	Super::EndPlay(EndPlayReason);
}

void ATanksterPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
}

void ATanksterPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
}

void ATanksterPlayerState::AmmoChanged(const FOnAttributeChangeData& Data)
{
}

void ATanksterPlayerState::MaxAmmoChanged(const FOnAttributeChangeData& Data)
{
}

float ATanksterPlayerState::GetHealth() const
{
	if(IsValid(HealthAttributeSet))
	{
		return HealthAttributeSet->GetHealth();
	}
	return 0.0f;
}

float ATanksterPlayerState::GetMaxHealth() const
{
	if(IsValid(HealthAttributeSet))
	{
		return HealthAttributeSet->GetMaxHealth();
	}
	return 0.0f;
}

float ATanksterPlayerState::GetAmmo() const
{
	if(IsValid(AmmoAttributeSet))
	{
		return AmmoAttributeSet->GetAmmo();
	}
	return 0.0f;
}

float ATanksterPlayerState::GetMaxAmmo() const
{
	if(IsValid(AmmoAttributeSet))
	{
		return AmmoAttributeSet->GetMaxAmmo();
	}
	return 0.0f;
}

UAbilitySystemComponent* ATanksterPlayerState::GetAbilitySystemComponent() const
{
	return TanksterAbilitySystem;
}

TWeakObjectPtr<UHealthAttributeSet> ATanksterPlayerState::GetHealthAttributeSet() const
{
	return HealthAttributeSet;
}

TWeakObjectPtr<UAmmoAttributeSet> ATanksterPlayerState::GetAmmoAttributeSet() const
{
	return AmmoAttributeSet;
}
