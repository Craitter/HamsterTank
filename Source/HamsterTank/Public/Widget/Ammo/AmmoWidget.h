// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

struct FOnAttributeChangeData;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	
	void OnAmmoChanged(const FOnAttributeChangeData& Data);
	void OnMaxAmmoChanged(const FOnAttributeChangeData& Data);

	void UpdateAmmo(int32 CurrentAmmo) const;
	void UpdateMaxAmmo(int32 CurrentMaxAmmo);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoCount = {nullptr};


	//Begin Health AttributeChangedDelegate
	FDelegateHandle AmmoChangedDelegateHandle;
	FDelegateHandle MaxAmmoChangedDelegateHandle;
	//End Health AttributeChangedDelegate

	FNumberFormattingOptions NumberFormattingOptions;
};
