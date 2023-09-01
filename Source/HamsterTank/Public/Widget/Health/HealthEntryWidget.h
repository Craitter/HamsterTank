// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthEntryWidget.generated.h"

class UImage;


UENUM()
enum class EHealthState : uint8
{
	Full,

	Half,
	
	Empty,
};
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UHealthEntryWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FullHealthIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HalfHealthIcon = nullptr;

	EHealthState CurrentHealthState = EHealthState::Full;

public:
	bool ShouldSkipUpdate(EHealthState NewState) const;
	void SetHealthState(EHealthState NewState);
	EHealthState GetCurrentHealthState() const;
};
