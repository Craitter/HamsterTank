// Fill out your copyright notice in the Description page of Project Settings.



#include "Widget/HUDWidgets/HUDHealthCountWidget.h"

#include "Components/HealthComponent.h"
#include "Components/Image.h"
#include "Components/MenuAnchor.h"

bool UHUDHealthCountWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	const TWeakObjectPtr<APawn> OwningPawn = GetOwningPlayerPawn();
	if(OwningPawn.IsValid())
	{
		const TWeakObjectPtr<UHealthComponent> PlayersHealthComponent = OwningPawn->FindComponentByClass<UHealthComponent>();
		if(PlayersHealthComponent.IsValid())
		{
			PlayersHealthComponent->OnHeathChangedDelegateHandle.AddUObject(this, &ThisClass::DisplayHealth);
			DisplayHealth(PlayersHealthComponent->GetCurrentHealth());
		}
	}

	

	return true;
}

bool UHUDHealthCountWidget::ShouldSkipUpdate() const
{
	return FirstHeart == nullptr || SecondHeart == nullptr || ThirdHeart == nullptr || ImageFullHeart == nullptr ||
		ImageHalfHeart == nullptr;
}

void UHUDHealthCountWidget::ResetImages() const
{
	FirstHeart->SetVisibility(ESlateVisibility::Hidden);
	SecondHeart->SetVisibility(ESlateVisibility::Hidden);
	ThirdHeart->SetVisibility(ESlateVisibility::Hidden);
}

void UHUDHealthCountWidget::DisplayHealth(const float NewHealth) const
{
	if(ShouldSkipUpdate())
	{
		return;
	}
	ResetImages();
	switch (FMath::RoundToInt32(NewHealth))
	{
	case 6:
		ShowHeartFull(FirstHeart);
		ShowHeartFull(SecondHeart);
		ShowHeartFull(ThirdHeart);
		break;
	case 5:
		ShowHeartFull(FirstHeart);
		ShowHeartFull(SecondHeart);
		ShowHeartHalf(ThirdHeart);
		break;
	case 4:
		ShowHeartFull(FirstHeart);
		ShowHeartFull(SecondHeart);
		break;
	case 3:
		ShowHeartFull(FirstHeart);
		ShowHeartHalf(SecondHeart);
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

void UHUDHealthCountWidget::ShowHeartFull(const TWeakObjectPtr<UImage> ImageToChange) const
{
	if(ImageToChange.IsValid() && ImageFullHeart != nullptr)
	{
		ImageToChange->SetBrushFromTexture(ImageFullHeart.Get());
		ImageToChange->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	
}

void UHUDHealthCountWidget::ShowHeartHalf(const TWeakObjectPtr<UImage> ImageToChange) const
{
	if(ImageToChange.IsValid() && ImageHalfHeart != nullptr)
	{
		ImageToChange->SetBrushFromTexture(ImageHalfHeart.Get());
		ImageToChange->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
