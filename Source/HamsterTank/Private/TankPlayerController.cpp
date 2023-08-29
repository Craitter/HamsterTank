// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ObjectiveSubsystem.h"
#include "TankHamsterGameInstance.h"
#include "Blueprint/UserWidget.h"

#include "Actors/TankBase.h"
#include "Components/AudioComponent.h"
#include "Components/HealthComponent.h"
#include "HamsterTank/HamsterTankGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Widget/TankBaseWidget.h"


ATankPlayerController::ATankPlayerController()
{
	bShowMouseCursor = false;
	const TObjectPtr<ULocalPlayer> DefaultLocalPlayer = ULocalPlayer::StaticClass()->GetDefaultObject<ULocalPlayer>();
	DefaultLocalPlayer->AspectRatioAxisConstraint = AspectRatio_MaintainYFOV;
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// if(TankPawn.IsValid())
	// {
	// 	FHitResult HitResult; 
	// 	if(GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, HitResult))
	// 	{
	// 		TankPawn->RequestAimAtTarget(HitResult.ImpactPoint);
	// 	}
	// 	else
	// 	{
	// 			UE_LOG(LogTemp, Warning , TEXT("Pointing into nothing isnt really working yet"));
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	// }
}

void ATankPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	ActiveWidget = CreateWidget<UTankBaseWidget>(this, InstructionsWidgetClass);
	if(ActiveWidget.IsValid())
	{
		
		ActiveWidget->AddToViewport();
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(ActiveWidget->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(Mode);
		DisableInput(this);
	}
	
	TankPawn = CastChecked<ATankBase>(InPawn);
	if(TankPawn.IsValid())
	{
		const TWeakObjectPtr<UHealthComponent> PawnHealthComponent = TankPawn->FindComponentByClass<UHealthComponent>();
		if(PawnHealthComponent.IsValid())
		{
			PawnHealthComponent->OnDeathDelegateHandle.AddUObject(this, &ThisClass::OnPlayerDied);
		}
	}
}
	

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	const TWeakObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer();
	if(!ensure(LocalPlayer.IsValid())) return;
	const TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(!ensure(EnhancedInputLocalPlayerSubsystem.IsValid())) return;
	if(IMC_MK_Default.IsNull())
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() IMC_MK_Default is Empty, Fill out BP_Controller"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
	else
	{
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_MK_Default.LoadSynchronous(), 0);
	}
	
	const TWeakObjectPtr<AHamsterTankGameModeBase> HamsterTankGameModeBase = Cast<AHamsterTankGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(HamsterTankGameModeBase.IsValid())
	{
		HamsterTankGameModeBase->HandleMatchEndDelegate.AddDynamic(this, &ThisClass::OnMatchIsEnding);
		HamsterTankGameModeBase->HandleMatchRestartDelegate.AddDynamic(this, &ThisClass::OnMatchRestart);
		HamsterTankGameModeBase->HandleMatchUnPauseDelegate.AddDynamic(this, &ThisClass::OnUnpauseMatch);
		HamsterTankGameModeBase->HandleMatchPauseDelegate.AddDynamic(this, &ThisClass::OnPauseMatch);
		HamsterTankGameModeBase->HandleMatchStartDelegate.AddDynamic(this, &ThisClass::OnMatchStart);
	}

	const TWeakObjectPtr<UTankHamsterGameInstance> TankHamsterGame = Cast<UTankHamsterGameInstance>(GetGameInstance());
	if(TankHamsterGame.IsValid())
	{
		TankHamsterGame->PlayBackgroundMusic(BackgroundMusic);
	}

	const TWeakObjectPtr<UGameInstance> GameInstance = GetGameInstance();
	if(GameInstance.IsValid())
	{
		ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
		if(ObjectiveSubsystem.IsValid())
		{
			ObjectiveSubsystem->RegisterPlayer(this);
		}
	}
}

void ATankPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(ObjectiveSubsystem.IsValid())
	{
		ObjectiveSubsystem->UnregisterPlayer(this);
	}
	
	if(EnhancedInputComponent.IsValid())
	{
		EnhancedInputComponent->RemoveBindingByHandle(DriveDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(DriveStopDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(FireDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(AimDelegateHandle);
		EnhancedInputComponent->RemoveBindingByHandle(PauseDelegateHandle);
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
	PauseDelegateHandle = EnhancedInputComponent->BindAction(IA_Pause, ETriggerEvent::Triggered, this, &ATankPlayerController::RequestPauseCallback).GetHandle();	
}

void ATankPlayerController::OnObjectiveTowerDestroyed()
{
	
}

void ATankPlayerController::OnMatchIsEnding()
{
	RemoveActiveWidget();

	ActiveWidget = CreateWidget<UTankBaseWidget>(this, VictoryWidgetClass);
	if(ActiveWidget.IsValid())
	{
		ActiveWidget->AddToViewport();
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(ActiveWidget->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(Mode);
	}
	
	DisableInput(this);
	if(PlayingBackgroundMusic.IsValid())
	{
		PlayingBackgroundMusic->Stop();
	}
	UGameplayStatics::PlaySound2D(this, VictorySound);
	if(TankPawn.IsValid())
	{
		TankPawn->ClearVelocity();
	}
	SetShowMouseCursor(true);
	
}

void ATankPlayerController::RemoveActiveWidget() const
{
	if(ActiveWidget.IsValid())
	{
		ActiveWidget->RemoveFromParent();
	}
}

void ATankPlayerController::OnPauseMatch()
{
	SetPause(true);
	SetShowMouseCursor(true);

	RemoveActiveWidget();
	ActiveWidget = CreateWidget<UTankBaseWidget>(this, PauseWidgetClass);
	if(ActiveWidget.IsValid())
	{
		ActiveWidget->AddToViewport();
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(ActiveWidget->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(Mode);
	}
}

void ATankPlayerController::OpenGameOverlay()
{
	RemoveActiveWidget();
	ActiveWidget = CreateWidget<UTankBaseWidget>(this, GameOverlayWidgetClass);
	if(ActiveWidget.IsValid())
	{
		ActiveWidget->AddToViewport();
	}
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
	EnableInput(this);
}

void ATankPlayerController::OnUnpauseMatch()
{
	SetPause(false);
	OpenGameOverlay();
}

void ATankPlayerController::OnMatchRestart()
{
	RestartLevel();
}

void ATankPlayerController::OnMatchStart()
{
	
	OpenGameOverlay();
}


void ATankPlayerController::SetMouseSensitivity(float NewValue)
{
	if(MouseSensityTranslation.GetRichCurveConst() == nullptr)
	{
		return;
	}
	const TWeakObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer();
	if(!ensure(LocalPlayer.IsValid())) return;
	const TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(!ensure(EnhancedInputLocalPlayerSubsystem.IsValid())) return;
	TArray<FEnhancedActionKeyMapping> Mappings = EnhancedInputLocalPlayerSubsystem->GetAllPlayerMappableActionKeyMappings();
	if(Mappings.IsEmpty())
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() EMPTY"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
	else
	{
		for (auto Mapping : Mappings)
		{
			if(Mapping.GetMappingName().IsEqual(TEXT("AimSensitivity")))
			{
				for (auto Modifier : Mapping.Modifiers)
				{
					if(Modifier.IsA(UInputModifierScalar::StaticClass()))
					{
						UInputModifierScalar* Scalar = Cast<UInputModifierScalar>(Modifier);
						if(Scalar != nullptr)
						{
							Scalar->Scalar = FVector(MouseSensityTranslation.GetRichCurveConst()->Eval(NewValue), 1.0f, 1.0f);
							break;
						}
					}
				}
			}
		}
	}
}

void ATankPlayerController::LoadMouseSensitivity()
{
	const TWeakObjectPtr<UTankHamsterGameInstance> GameInstance = Cast<UTankHamsterGameInstance>(GetGameInstance());
	if(GameInstance.IsValid())
	{
		SetMouseSensitivity(GameInstance->GetMouseSensivity());
	}
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
	//We can not use control rotation @see TankSpringArmComponent
	if(TankPawn.IsValid())
	{
		const TWeakObjectPtr<USceneComponent> Tower = TankPawn->GetTower();
		// const TWeakObjectPtr<USpringArmComponent> SpringArm = TankPawn->GetSpringArm();
		if(Tower.IsValid()) //&& SpringArm.IsValid())
		{
			// Tower->AddRelativeRotation(FRotator(0.0f, Value.Get<float>(), 0.0f));
			AddYawInput(Value.Get<float>());
			
			// SpringArm->AddRelativeRotation(FRotator(0.0, Value.Get<float>(), 0.0f));
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
}

void ATankPlayerController::RequestPauseCallback()
{
	const TWeakObjectPtr<AHamsterTankGameModeBase> HamsterTankGameModeBase = Cast<AHamsterTankGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(HamsterTankGameModeBase.IsValid())
	{
		HamsterTankGameModeBase->PauseMatch();
	}
}

void ATankPlayerController::OnPlayerDied(TWeakObjectPtr<AController> DamageInstigator)
{
	if(PlayingBackgroundMusic.IsValid())
	{
		PlayingBackgroundMusic->Stop();
	}
	UGameplayStatics::PlaySound2D(this, DefeatSound);
	DisableInput(this);

	RemoveActiveWidget();
	ActiveWidget = CreateWidget<UTankBaseWidget>(this, DefeatWidgetClass);
	if(ActiveWidget.IsValid())
	{
		ActiveWidget->AddToViewport();
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(ActiveWidget->TakeWidget());
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(Mode);
	}
	//todo restart/open menu
}

