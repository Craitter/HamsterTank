// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "TankPlayerController.generated.h"

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

	void OnObjectiveTowerDestroyed();

	void OnGameIsEnding(FObjectiveScore& Score);
protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupInputComponent() override;

	

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Drive = {nullptr};
		
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Fire = {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_ChangeSpringArmDirection = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Aim = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_EnableCameraRotation = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Pause = {nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> IMC_MK_Default = {nullptr};
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTankBaseWidget> GameOverlayClass = {nullptr};
private:
	void RequestDriveCallback(const FInputActionValue& Value);

	void RequestFireCallback();

	void RequestAimCallback(const FInputActionValue& Value);

	void RequestPauseCallback();
	void RequestUnPauseCallback();
	
	void OnPlayerDied(TWeakObjectPtr<AController> DamageInstigator);
	

	TWeakObjectPtr<UGameOverlayWidget> GameOverlayWidget = {nullptr};
	TWeakObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = {nullptr};
	TWeakObjectPtr<ATankBase> TankPawn = {nullptr};
	TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem ={nullptr};
	TWeakObjectPtr<UUISubsystem> UISubsystem = {nullptr};
	
	uint32 DriveDelegateHandle = 0;
	uint32 DriveStopDelegateHandle = 0;
	uint32 FireDelegateHandle = 0;
	uint32 AimDelegateHandle = 0;
	uint32 PauseDelegateHandle = 0;
};


