// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TankBase.h"

ATankPlayerController::ATankPlayerController()
{
	bShowMouseCursor = true;
	const TObjectPtr<ULocalPlayer> DefaultLocalPlayer = ULocalPlayer::StaticClass()->GetDefaultObject<ULocalPlayer>();
	DefaultLocalPlayer->AspectRatioAxisConstraint = AspectRatio_MaintainYFOV;
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(TankPawn.IsValid())
	{
		FHitResult HitResult; 
		if(GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, HitResult))
		{
			TankPawn->RequestAimAtTarget(HitResult.ImpactPoint);
		}
		else
		{
			
			// FVector MouseLocation;
			// FVector MouseDirection;
			// // MouseP
			// if(DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
			// {
				UE_LOG(LogTemp, Warning , TEXT("Pointing into nothing isnt really working yet"));
			// 	TankPawn->RequestAimAtTarget(MouseLocation);
			// }
		
			// UE_LOG(LogTemp, Warning , TEXT("Start, End "));
			// TankPawn->RequestAimAtTarget(HitResult.);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
}

void ATankPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TankPawn = CastChecked<ATankBase>(InPawn);
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const TWeakObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer();
	if(!ensure(LocalPlayer.IsValid())) return;
	const TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(!ensure(LocalPlayer.IsValid())) return;
	if(IMC_MK_Default.IsNull())
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() IMC_MK_Default is Empty, Fill out BP_Controller"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
	else
	{
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_MK_Default.LoadSynchronous(), 0);	
	}
}

void ATankPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(EnhancedInputComponent.IsValid())
	{
		EnhancedInputComponent->RemoveBindingByHandle(DriveDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(DriveStopDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(FireDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(AimDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(EnableCameraRotationDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(DisableCameraRotationDelegateHandle);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	DriveDelegateHandle = EnhancedInputComponent->BindAction(IA_Drive, ETriggerEvent::Triggered, this, &ATankPlayerController::RequestDriveCallback).GetHandle();
	DriveStopDelegateHandle = EnhancedInputComponent->BindAction(IA_Drive, ETriggerEvent::Completed, this, &ATankPlayerController::RequestDriveCallback).GetHandle();
	FireDelegateHandle = EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ATankPlayerController::RequestFireCallback).GetHandle();
	AimDelegateHandle = EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Triggered, this, &ATankPlayerController::RequestAimCallback).GetHandle();
	EnableCameraRotationDelegateHandle = EnhancedInputComponent->BindAction(IA_EnableCameraRotation, ETriggerEvent::Started, this, &ATankPlayerController::RequestEnableCameraCallback).GetHandle();
	DisableCameraRotationDelegateHandle = EnhancedInputComponent->BindAction(IA_EnableCameraRotation, ETriggerEvent::Completed, this, &ATankPlayerController::RequestEnableCameraCallback).GetHandle();
}

void ATankPlayerController::RequestDriveCallback(const FInputActionValue& Value)
{
	if(TankPawn.IsValid())
	{
		const FVector2D DrivingInput = Value.Get<FVector2D>();
		TankPawn->AddMovementInput({DrivingInput.X, DrivingInput.Y, 0.0f});
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
}

void ATankPlayerController::RequestFireCallback()
{
	if(TankPawn.IsValid())
	{
		TankPawn->RequestFire();
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
}

void ATankPlayerController::RequestAimCallback(const FInputActionValue& Value)
{
	if(bAddControlRotation)
	{
		AddYawInput(Value.Get<float>());
	}
}

void ATankPlayerController::RequestEnableCameraCallback(const FInputActionValue& Value)
{
	if(TankPawn.IsValid())
	{
		
		const bool bCameraEnabled = Value.Get<bool>();
		bAddControlRotation = bCameraEnabled;
		TankPawn->RequestEnableCameraRotation(bCameraEnabled);
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
	
}

