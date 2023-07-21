// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/GameOverlayWidget.h"

#include "FireProjectileComponent.h"
#include "Components/DynamicEntryBox.h"
#include "Widget/BulletIconWidget.h"

void UGameOverlayWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	const TWeakObjectPtr<APawn> Pawn = GetOwningPlayerPawn();
	if(Pawn.IsValid())
	{
		const TWeakObjectPtr<UFireProjectileComponent> FireProjectileComponent = Pawn->FindComponentByClass<UFireProjectileComponent>();
		if(FireProjectileComponent.IsValid())
		{
			OnMaxAmmoChanged(FireProjectileComponent->GetMaxAmmo(), FireProjectileComponent->GetCurrentAmmo());
			FireProjectileComponent->OnAmmoChanged.AddUObject(this, &ThisClass::OnAmmoChanged);
			FireProjectileComponent->OnMaxAmmoChanged.AddUObject(this, &ThisClass::OnMaxAmmoChanged);
		}
	}
	
}

void UGameOverlayWidget::OnAmmoChanged(int32 NewAmmo)
{
	if(AmmoEntryBox != nullptr)
	{
		for(int32 i = AmmoEntries.Num() - 1; i >= 0; i--)
		{
			if(AmmoEntries.IsValidIndex(i))
			{
				AmmoEntries[i]->SetIsFilledAmmo(NewAmmo > 0);
				NewAmmo--;
			}
		}
	}
}

void UGameOverlayWidget::OnMaxAmmoChanged(int32 NewMaxAmmo, int32 CurrentAmmo)
{

	if(AmmoEntryBox != nullptr)
	{
		//Clear Entries since we have to recreate it all (We could also change depending on the current entries, but this way we can use it in anyway)
		if(!AmmoEntries.IsEmpty())
		{
			for (TObjectPtr<UBulletIconWidget> Entry : AmmoEntries)
			{
				AmmoEntryBox->RemoveEntry(Entry);
			}
			AmmoEntries.Empty();
		}
		int32 SwitchIndex = CurrentAmmo;
		for (int32 i = 0; i < NewMaxAmmo; i++)
		{
			TObjectPtr<UBulletIconWidget> BulletIcon = AmmoEntryBox->CreateEntry<UBulletIconWidget>();
			if(BulletIcon != nullptr)
			{
				if(SwitchIndex > 0)
				{
					BulletIcon->SetIsFilledAmmo(true);
					SwitchIndex--;
				}
				AmmoEntries.Add(BulletIcon);
			}
			else
			{
				UE_LOG(LogTemp, Error , TEXT("%s Created Entry was null"), *FString(__FUNCTION__));
			}
		}
	}
}
