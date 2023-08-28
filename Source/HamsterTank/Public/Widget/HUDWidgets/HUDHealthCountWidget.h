// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "HUDHealthCountWidget.generated.h"

class USpacer;
class UMenuAnchor;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHUDHealthCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	
	bool ShouldSkipUpdate() const;
	void ResetImages() const;
	
	void DisplayHealth(float NewHealth) const;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FirstHeart = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SecondHeart = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ThirdHeart = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> ImageFullHeart = {nullptr};
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> ImageHalfHeart = {nullptr};

	void ShowHeartFull(TWeakObjectPtr<UImage> ImageToChange) const;

	void ShowHeartHalf(TWeakObjectPtr<UImage> ImageToChange) const;
};
