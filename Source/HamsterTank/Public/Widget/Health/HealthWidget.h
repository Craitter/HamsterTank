// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

class UHealthEntryWidget;
struct FOnAttributeChangeData;
class UDynamicEntryBox;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	virtual void NativeDestruct() override;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> HealthIcons = {nullptr};

	TArray<UHealthEntryWidget*> HealthWidgets;

	//Begin Health AttributeChangedDelegate
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	//End Health AttributeChangedDelegate
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	void UpdateMaxHealth(int32 NewMaxHealth);
	void UpdateHealth(int32 NewHealth);
	
};
