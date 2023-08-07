// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BulletIconWidget.h"

#include "Components/FireProjectileComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


bool UBulletIconWidget::Initialize()
{
	return Super::Initialize();
}

void UBulletIconWidget::NativePreConstruct()
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

void UBulletIconWidget::OnAmmoChanged(int32 NewAmmo)
{
	if(AmmoCount != nullptr)
	{
		const FText Count = FText::FromString("{Amount}x");
		FFormatNamedArguments Args;
		Args.Add(TEXT("Amount"), NewAmmo);
		AmmoCount->SetText(FText::Format(Count, Args));
	}
}

void UBulletIconWidget::OnMaxAmmoChanged(int32 NewMaxAmmo, int32 CurrentAmmo)
{
}
