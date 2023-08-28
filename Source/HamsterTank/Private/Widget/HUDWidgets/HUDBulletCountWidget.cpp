// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUDWidgets/HUDBulletCountWidget.h"

#include "Components/FireProjectileComponent.h"
#include "Components/TextBlock.h"


bool UHUDBulletCountWidget::Initialize()
{
	if(!Super::Initialize()) return false;
	
	const TWeakObjectPtr<APawn> OwningPawn = GetOwningPlayerPawn();
	if(!ensure(OwningPawn != nullptr)) return false;
	const TWeakObjectPtr<UFireProjectileComponent> FireProjectileComponent = OwningPawn->FindComponentByClass<UFireProjectileComponent>();
	if(!ensure(FireProjectileComponent != nullptr)) return false;
	
	FireProjectileComponent->OnAmmoChanged.AddUObject(this, &ThisClass::OnAmmoChanged);
	OnAmmoChanged(FireProjectileComponent->GetCurrentAmmo());
	
	return true;
}


void UHUDBulletCountWidget::OnAmmoChanged(const int32 NewAmmo) const
{
	if(AmmoCount != nullptr)
	{
		const FText Count = FText::FromString("{Amount}x");
		
		FFormatNamedArguments Args;
		Args.Add(TEXT("Amount"), NewAmmo);
		
		AmmoCount->SetText(FText::Format(Count, Args));
	}
}

