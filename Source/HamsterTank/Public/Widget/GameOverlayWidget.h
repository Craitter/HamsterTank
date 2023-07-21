// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverlayWidget.generated.h"

class UBulletIconWidget;
class UDynamicEntryBox;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UGameOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativePreConstruct() override;
	void OnAmmoChanged(int32 NewAmmo);
	void OnMaxAmmoChanged(int32 NewMaxAmmo, int32 CurrentAmmo);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> AmmoEntryBox = {nullptr};


	TArray<TObjectPtr<UBulletIconWidget>> AmmoEntries;
};
