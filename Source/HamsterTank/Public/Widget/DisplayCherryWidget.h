// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DisplayCherryWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UDisplayCherryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	void OnCherryCountChanged(float NewCherryCount);
protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CherryImage = {nullptr};

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CherryCount = {nullptr};
};
