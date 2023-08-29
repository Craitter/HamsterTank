// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDCherryCountWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHUDCherryCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UFUNCTION()
	void OnCherryCountChanged(int32 NewCherryCount) const;
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CherryImage = {nullptr};

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CherryCount = {nullptr};
};
