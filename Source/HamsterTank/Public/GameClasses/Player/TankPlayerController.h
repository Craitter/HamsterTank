// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "TankPlayerController.generated.h"

class UTanksterInputConfig;
class UTanksterEnhancedInputComponent;
class UPauseMenuWidget;
class USoundCue;
struct FObjectiveScore;
class UTankBaseWidget;
class UUISubsystem;
class UGameOverlayWidget;
class UObjectiveSubsystem;
class USphereComponent;
class UCameraComponent;
class USpringArmComponent;
class ATankBase;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */

UCLASS()
class HAMSTERTANK_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATankPlayerController();

	virtual void Tick(float DeltaSeconds) override;

	
protected:
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void SetupInputComponent() override;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	void Input_Drive(const FInputActionValue& Value);
	void Input_LookMouse(const FInputActionValue& Value);
	void Input_LookStick(const FInputActionValue& Value);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UTanksterInputConfig> TanksterInputConfig = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> IMC_MK_Default = {nullptr};
	

private:
	
	void OnPlayerDied(TWeakObjectPtr<AController> DamageInstigator);
	
	TWeakObjectPtr<UTanksterEnhancedInputComponent> TanksterInputComponent = {nullptr};
	TWeakObjectPtr<ATankBase> TankPawn = {nullptr};

	UPROPERTY()
	TArray<uint32> BindHandles;
	
};


