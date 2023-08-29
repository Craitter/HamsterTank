// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TankBaseWidget.h"
#include "GenericSliderWidget.generated.h"

class UButton;
class UTextBlock;
class USlider;


UENUM()
enum class ESliderType : uint8
{
	None,
	
	MasterVolume,

	MouseSensitivity,
};
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UGenericSliderWidget : public UTankBaseWidget
{
	GENERATED_BODY()
public:

	virtual bool Initialize() override;

	virtual void NativePreConstruct() override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USlider> Slider = {nullptr};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> NameTextBlock = {nullptr};

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueTextBlock = {nullptr};

	UPROPERTY(EditAnywhere)
	ESliderType SliderType = ESliderType::None;
	
	UPROPERTY(EditAnywhere, Category = "Slider")
	FText SliderName = FText::GetEmpty();
	
	UPROPERTY(EditAnywhere, Category = "Slider")
	float MinValue = 1;
	UPROPERTY(EditAnywhere, Category = "Slider")
	float MaxValue = 1;
	UPROPERTY(EditAnywhere, Category = "Slider")
	float StepSize = 1;
	
	UPROPERTY(EditAnywhere, Category = "Slider")
	int32 MinIntegralNum = 1;
	UPROPERTY(EditAnywhere, Category = "Slider")
	int32 MaxIntegralNum = 1;
	UPROPERTY(EditAnywhere, Category = "Slider")
	int32 MinFractionalNum = 1;
	UPROPERTY(EditAnywhere, Category = "Slider")
	int32 MaxFractionalNum = 1;
	
	
	virtual float LoadCurrentValue();


	virtual void HandleValueChanged(float NewValue);

	UFUNCTION()
	void OnValueChanged(float NewValue);
protected:

	void UpdateDisplayedValue(float NewValue) const;

	FNumberFormattingOptions ValueFormatOptions;
};
