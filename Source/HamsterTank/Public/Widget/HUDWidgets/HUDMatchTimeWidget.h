// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDMatchTimeWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHUDMatchTimeWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual bool Initialize() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayTime = {nullptr};

	float InitialTimeDelay = 0.0f;
	FNumberFormattingOptions NumberFormattingOptions;
};
