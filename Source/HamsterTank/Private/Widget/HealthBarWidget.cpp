// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HealthBarWidget.h"

#include "Components/HealthComponent.h"
#include "Components/MenuAnchor.h"

void UHealthBarWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(GetOwningPlayerPawn() != nullptr)
	{
		const TWeakObjectPtr<UHealthComponent> PlayersHealthComponent = GetOwningPlayerPawn()->FindComponentByClass<UHealthComponent>();
		if(PlayersHealthComponent.IsValid())
		{
			PlayersHealthComponent->OnHeathChangedDelegateHandle.AddUObject(this, &ThisClass::DisplayHealth);
			DisplayHealth(PlayersHealthComponent->GetCurrentHealth());
		}
	}
}

void UHealthBarWidget::NativeDestruct()
{
	if(GetOwningPlayerPawn() != nullptr)
	{
		const TWeakObjectPtr<UHealthComponent> PlayersHealthComponent = GetOwningPlayerPawn()->FindComponentByClass<UHealthComponent>();
		if(PlayersHealthComponent.IsValid())
		{
			PlayersHealthComponent->OnHeathChangedDelegateHandle.RemoveAll(this);
		}
	}
	Super::NativeDestruct();
}

void UHealthBarWidget::DisplayHealth(float NewHealth)
{
	if(FirstHeart == nullptr || MiddleHeart == nullptr || LastHeart == nullptr || FullHeartWidget == nullptr || HalfHeartWidget == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning , TEXT("called"));
	const int32 Health = FMath::RoundToInt32(NewHealth);
	UE_LOG(LogTemp, Warning , TEXT("health%d"), Health);
	FirstHeart->SetContent(nullptr);
	MiddleHeart->SetContent(nullptr);
	LastHeart->SetContent(nullptr);
	switch (Health)
	{
	case 6:
		ShowHeartFull(FirstHeart);
		ShowHeartFull(MiddleHeart);
		ShowHeartFull(LastHeart);
		break;
	case 5:
		ShowHeartFull(FirstHeart);
		ShowHeartFull(MiddleHeart);
		ShowHeartHalf(LastHeart);
		break;
	case 4:
		ShowHeartFull(FirstHeart);
		ShowHeartFull(MiddleHeart);
		break;
	case 3:
		ShowHeartFull(FirstHeart);
		ShowHeartHalf(MiddleHeart);
		break;
	case 2:
		ShowHeartFull(FirstHeart);
		break;
	case 1:
		ShowHeartHalf(FirstHeart);
		break;
	default:;
	}
}

void UHealthBarWidget::ShowHeartFull(TWeakObjectPtr<UMenuAnchor> HeartAnchor)
{
	if(HeartAnchor.IsValid() && FullHeartWidget != nullptr)
	{
		const TObjectPtr<UUserWidget> HeartWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), FullHeartWidget);
		if(HeartWidget != nullptr)
		{
			HeartAnchor->SetContent(HeartWidget);
			HeartAnchor->Open(false);
		}
	}
	
}

void UHealthBarWidget::ShowHeartHalf(TWeakObjectPtr<UMenuAnchor> HeartAnchor)
{
	if(HeartAnchor.IsValid() && HalfHeartWidget != nullptr)
	{
		const TObjectPtr<UUserWidget> HeartWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HalfHeartWidget);
		if(HeartWidget != nullptr)
		{
			HeartAnchor->SetContent(HeartWidget);
			HeartAnchor->Open(false);
		}
	}
}
