// Fill out your copyright notice in the Description page of Project Settings.



#include "GameClasses/Player/TankPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/TanksterAbilitySystemComponent.h"
#include "Actors/TankBase.h"
#include "Components/HealthComponent.h"
#include "GameClasses/TanksterGameplayTags.h"
#include "Input/TanksterEnhancedInputComponent.h"


ATankPlayerController::ATankPlayerController()
{
	bShowMouseCursor = false;
	const TObjectPtr<ULocalPlayer> DefaultLocalPlayer = ULocalPlayer::StaticClass()->GetDefaultObject<ULocalPlayer>();
	DefaultLocalPlayer->AspectRatioAxisConstraint = AspectRatio_MaintainYFOV;
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}



void ATankPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
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
}

void ATankPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
		
	if(TanksterInputComponent.IsValid())
	{
		TanksterInputComponent->RemoveBinds(BindHandles);
	}
	
	Super::EndPlay(EndPlayReason);
}

void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	TanksterInputComponent = CastChecked<UTanksterEnhancedInputComponent>(InputComponent);
	const TWeakObjectPtr<ULocalPlayer> LocalPlayer = GetLocalPlayer();
	
	check(TanksterInputConfig)
	check(LocalPlayer.Get())
	
	const TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EnhancedInputLocalPlayerSubsystem.Get())
	
	EnhancedInputLocalPlayerSubsystem->ClearAllMappings();
	
	if(IMC_MK_Default.IsNull())
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() IMC_MK_Default is Empty, Fill out BP_Controller"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
	else
	{
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_MK_Default.LoadSynchronous(), 0);
	}

	const FTanksterGameplayTags& GameplayTags = FTanksterGameplayTags::Get();
	
	
	TanksterInputComponent->BindAbilityActions(TanksterInputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

	TanksterInputComponent->BindNativeAction(TanksterInputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Drive, /*bLogIfNotFound=*/ false, BindHandles);
	TanksterInputComponent->BindNativeAction(TanksterInputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false, BindHandles);
	TanksterInputComponent->BindNativeAction(TanksterInputConfig, GameplayTags.InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false, BindHandles);
}

void ATankPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);

	
	if(TankPawn.IsValid() && TankPawn->GetTanksterAbilitySystemComponent().IsValid())
	{
		TankPawn->GetTanksterAbilitySystemComponent()->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
}


void ATankPlayerController::Input_Drive(const FInputActionValue& Value)
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

// void ATankPlayerController::RequestFireCallback()
// {
// 	if(TankPawn.IsValid())
// 	{
// 		TankPawn->RequestFire();
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
// 	}
// }

void ATankPlayerController::Input_LookMouse(const FInputActionValue& Value)
{
	AddYawInput(Value.Get<float>());
}

void ATankPlayerController::Input_LookStick(const FInputActionValue& Value)
{
	
}

void ATankPlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(TankPawn.IsValid())
	{
		TWeakObjectPtr<UTanksterAbilitySystemComponent> ASC = TankPawn->GetTanksterAbilitySystemComponent();
		if(ASC.IsValid())
		{
			ASC->AbilityInputTagPressed(InputTag);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
}

void ATankPlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(TankPawn.IsValid())
	{
		TWeakObjectPtr<UTanksterAbilitySystemComponent> ASC = TankPawn->GetTanksterAbilitySystemComponent();
		if(ASC.IsValid())
		{
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning , TEXT("%s %s() No Valid Pawn to Forward Input to"), *UEnum::GetValueAsString(GetLocalRole()), *FString(__FUNCTION__));
	}
}


void ATankPlayerController::OnPlayerDied(TWeakObjectPtr<AController> DamageInstigator)
{
	DisableInput(this);
}

