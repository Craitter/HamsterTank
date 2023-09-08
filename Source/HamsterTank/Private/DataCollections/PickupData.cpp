// Fill out your copyright notice in the Description page of Project Settings.


#include "DataCollections/PickupData.h"

#include "Actors/TankBase.h"
#include "Kismet/GameplayStatics.h"

UPickupData::UPickupData()
{
	
}

float UPickupData::CalculateSum()
{
	float OutSum = 0.0f;
	for (const FPickupData_Backend& Pickup : Pickups)
	{
		OutSum += Pickup.WeightedProbability;
	}
	return OutSum;
}

EPickupType UPickupData::DoesPlayerNeedResources()
{
	ForcePickups.Sort();
	const TWeakObjectPtr<ATankBase> TankBase = Cast<ATankBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	if(TankBase.IsValid())
	{
		for (const auto& Pickup : ForcePickups)
		{
			switch (Pickup.Type)
			{
			case EPickupType::Ammo:
				if(NeedsAmmo(TankBase))
				{
					return EPickupType::Ammo;
				}
				break;
			case EPickupType::Heal:
				if(NeedsHealth(TankBase))
				{
					return EPickupType::Heal;
				}
				break;
			default: ;
			}
		}
	}
	return EPickupType::None;
}

bool UPickupData::NeedsAmmo(TWeakObjectPtr<ATankBase> PlayerTank)
{
	return PlayerTank->GetAmmo() < 5.0f;
}

bool UPickupData::NeedsHealth(TWeakObjectPtr<ATankBase> PlayerTank)
{
	return PlayerTank->GetHealth() < 1.0f;
}

FPickupData_Backend* UPickupData::ChoosePickup()
{
	float RandomValue = FMath::FRandRange(0.0f, CalculateSum());
	const EPickupType PickupType = DoesPlayerNeedResources();
	const bool bPlayerNeedsResources = PickupType == EPickupType::None ? false : true;
	
	for (FPickupData_Backend& Pickup : Pickups)
	{
		if(bPlayerNeedsResources)
		{
			if(Pickup.Type == PickupType)
			{
				return &Pickup;
			}
			continue;
		}
		if(Pickup.WeightedProbability > RandomValue)
		{
			return &Pickup;	
		}
		else
		{
			RandomValue -= Pickup.WeightedProbability;
		}
	}
	return nullptr;
}
