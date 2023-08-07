// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/GameOverlayWidget.h"

#include "ObjectiveSubsystem.h"
#include "TankHamsterGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Widget/DefeatWidget.h"
#include "Widget/PauseMenuWidget.h"
#include "Widget/UISubsystem.h"
#include "Widget/VictoryWidget.h"


bool UGameOverlayWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(!ensure(InstructionsButton != nullptr)) return false;

	InstructionsButton->OnClicked.AddDynamic(this, &ThisClass::OnInstructionsRead);
	
	
	return true;
}

void UGameOverlayWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	GameInstance = Cast<UTankHamsterGameInstance>(GetGameInstance());
	if(GameInstance.IsValid())
	{
		const TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GameInstance->GetSubsystem<UObjectiveSubsystem>();
		if(ObjectiveSubsystem.IsValid())
		{
			TMulticastDelegate<void(float)>* OnScoreChangedDelegate = ObjectiveSubsystem->GetOnScoreChangedDelegate(GetOwningPlayer());
			if(OnScoreChangedDelegate != nullptr)
			{
				OnScoreChangedDelegate->AddUObject(this, &ThisClass::OnScoreChanged);
			}
			ObjectiveSubsystem->OnTowerDestroyed.AddUObject(this, &ThisClass::DisplayKill);
		}
		if(UISubsystem.IsValid())
		{
			UISubsystem->OnPlayerDeath.AddUObject(this, &ThisClass::OnPlayerDeath);
			UISubsystem->OnGameWon.AddUObject(this, &ThisClass::OnGameWon);
			UISubsystem->PauseGameDelegate.AddUObject(this, &ThisClass::OnPauseGame);
			UISubsystem->UnPauseGameDelegate.AddUObject(this, &ThisClass::OnUnPauseGame);
		}
	}
	if(PlayTime != nullptr)
	{
		PlayTime->SetVisibility(ESlateVisibility::Hidden);
	}
	PlayAnimation(ContinueAnimation, 0, 200);
}

void UGameOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	check(GetWorld())
	if(PlayTime != nullptr && UISubsystem.IsValid())
	{
		
		const FTimespan Time = FTimespan::FromSeconds(GetWorld()->GetTimeSeconds() - UISubsystem->GetGameTimeDelay());
		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.AlwaysSign = false;
		NumberFormattingOptions.UseGrouping = true;
		NumberFormattingOptions.MinimumIntegralDigits = 2;
		NumberFormattingOptions.MaximumIntegralDigits = 2;
		const FText Minutes = FText::AsNumber(Time.GetMinutes(), &NumberFormattingOptions);
		const FText Seconds = FText::AsNumber(Time.GetSeconds(), &NumberFormattingOptions);
		const FText TimeText = FText::FromString("{m}:{s}");
		
		FFormatNamedArguments Args;
		Args.Add("m", Minutes);
		Args.Add("s", Seconds);
		PlayTime->SetText(FText::Format(TimeText, Args));
	}
}

void UGameOverlayWidget::OnScoreChanged(float NewScore)
{
	check(GetWorld())
	if(InGameScore != nullptr)
	{
		TargetScore = NewScore;
		if(!GetWorld()->GetTimerManager().IsTimerActive(ScoreIncreaseHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(ScoreIncreaseHandle, this, &ThisClass::UpdateScore, ScoreIncreaseTimeSteps, true);
		}
	}
}

void UGameOverlayWidget::DisplayKill()
{
	if(DisplayKillAnimation != nullptr)
	{
		PlayAnimationForward(DisplayKillAnimation);
	}
}

FReply UGameOverlayWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if(InstructionsButton->OnClicked.IsBound())
	{
		OnInstructionsRead();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UGameOverlayWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(InstructionsButton->OnClicked.IsBound())
	{
		OnInstructionsRead();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGameOverlayWidget::UpdateScore()
{
	check(GetWorld())
	if(InGameScore != nullptr)
	{
		float CurrentScore = 0.0f;
		LexFromString(CurrentScore, *InGameScore->GetText().ToString());
		UE_LOG(LogTemp, Warning , TEXT("score %f"), CurrentScore);
		float DeltaThisStep = 0.0f;
		do
		{
			CurrentScore += ScoreIncreaseSteps;
			DeltaThisStep += ScoreIncreaseSteps;
			InGameScore->SetText(FText::AsNumber(CurrentScore));
			if(TargetScore < CurrentScore + UE_FLOAT_NORMAL_THRESH)
			{
				GetWorld()->GetTimerManager().ClearTimer(ScoreIncreaseHandle);
				break;
			}
		}
		while (DeltaThisStep > ScoreDeltaPerStep);
	
	}
}

void UGameOverlayWidget::OnGameWon()
{
	if(Switcher != nullptr)
	{
		Switcher->SetActiveWidget(Victory);
		Victory->PlayAnimationForward(Victory->VictoryAnimation);
		
		
		if(GetOwningPlayer())
		{
			GetOwningPlayer()->SetPause(true);
			GetOwningPlayer()->SetShowMouseCursor(true);
			FInputModeUIOnly ModeUIOnly;
			ModeUIOnly.SetWidgetToFocus(TakeWidget());
			ModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			GetOwningPlayer()->SetInputMode(ModeUIOnly);
		}
	}
}

void UGameOverlayWidget::OnPlayerDeath()
{
	if(Switcher != nullptr)
	{
		Switcher->SetActiveWidget(Defeat);
		if(GetOwningPlayer())
		{
			GetOwningPlayer()->SetShowMouseCursor(true);
			FInputModeUIOnly ModeUIOnly;
			ModeUIOnly.SetWidgetToFocus(TakeWidget());
			ModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			GetOwningPlayer()->SetInputMode(ModeUIOnly);
		}
	}
}

void UGameOverlayWidget::OnPauseGame()
{
	if(Switcher != nullptr)
	{
		Switcher->SetActiveWidget(PauseMenu);
	}
}

void UGameOverlayWidget::OnUnPauseGame()
{
	if(Switcher != nullptr)
	{
		Switcher->SetActiveWidgetIndex(1);
	}
}

void UGameOverlayWidget::OnInstructionsRead()
{
	if(Switcher != nullptr)
	{
		GetOwningPlayer()->SetShowMouseCursor(false);
		GetOwningPlayer()->SetInputMode(FInputModeGameOnly());
		StopAnimation(ContinueAnimation);
		InstructionsButton->OnClicked.Clear();
		Switcher->SetActiveWidgetIndex(1);
		StartGameAnimationEvent.BindDynamic(this, &ThisClass::OnStartGameAnimationFinished);
		BindToAnimationFinished(StartGameAnimation, StartGameAnimationEvent);
		PlayAnimationForward(StartGameAnimation);
	}
}

void UGameOverlayWidget::OnStartGameAnimationFinished()
{
	if(GetOwningPlayer())
	{
		
		GetOwningPlayer()->EnableInput(GetOwningPlayer());
		

		if(PlayTime && UISubsystem.IsValid())
		{
			UISubsystem->AddTimeDelay(GetWorld()->GetTimeSeconds());
			PlayTime->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
