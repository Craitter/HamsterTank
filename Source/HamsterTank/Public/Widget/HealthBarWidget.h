// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UMenuAnchor;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	virtual void NativeDestruct() override;
	void DisplayHealth(float NewHealth);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuAnchor> FirstHeart = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuAnchor> MiddleHeart = {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMenuAnchor> LastHeart = {nullptr};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HalfHeartWidget = {nullptr};
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> FullHeartWidget = {nullptr};

	void ShowHeartFull(TWeakObjectPtr<UMenuAnchor> HeartAnchor);

	void ShowHeartHalf(TWeakObjectPtr<UMenuAnchor> HeartAnchor);
};
