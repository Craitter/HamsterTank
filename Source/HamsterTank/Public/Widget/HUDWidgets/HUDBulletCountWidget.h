// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDBulletCountWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHUDBulletCountWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	void OnAmmoChanged(int32 NewAmmo) const;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AmmoImage = {nullptr};

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoCount = {nullptr};


public:
};
