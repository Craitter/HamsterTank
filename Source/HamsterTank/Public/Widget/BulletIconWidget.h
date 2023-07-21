// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BulletIconWidget.generated.h"

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

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CurrentBulletImage = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> FilledAmmo = {nullptr};
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> UsedAmmo = {nullptr};

	bool bIsFilled;

public:
	

	void SetIsFilledAmmo(bool bNewState);
};
