// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletIconWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UBulletIconWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

	virtual void NativePreConstruct() override;
	void OnAmmoChanged(int32 NewAmmo);
	void OnMaxAmmoChanged(int32 NewMaxAmmo, int32 CurrentAmmo);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AmmoImage = {nullptr};

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoCount = {nullptr};


public:
};
