// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CalculatePointsWidget.h"

#include "ObjectiveSubsystem.h"
#include "TankHamsterGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Widget/LeaderboardWidget.h"
#include "Widget/OverlayButtonsWidget.h"
#include "Widget/UISubsystem.h"

void UCalculatePointsWidget::Start()
{
	if(GameInstance.IsValid())
	{
		const TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GameInstance->GetSubsystem<UObjectiveSubsystem>();
		if(ObjectiveSubsystem.IsValid())
		{
			Score = ObjectiveSubsystem->GetScore(GetOwningPlayer());
			CalculateNext();
		}
	}	
}

void UCalculatePointsWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(Buttons)
	{
		Buttons->ShowResumeButton(false);
	}
	if(Confirm)
	{
		Confirm->OnClicked.AddDynamic(this, &ThisClass::OnNameConfirmed);
	}
}

void UCalculatePointsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
	if(ShouldSkipUpdate())
	{
		return;
	}
	if(CurrentStepSize < 1.0f)
	{
		CurrentStepSize = 1.0f;
	}
	CurrentValue = FMath::Clamp(CurrentValue + CurrentStepSize, 0.0f, CurrentTopEnd);
	CurrentTextBlock->SetText(FText::AsNumber(FMath::RoundToInt32(CurrentValue)));
	if(CurrentValue + UE_FLOAT_NORMAL_THRESH >=  CurrentTopEnd)
	{
		CalculateNext();
	}
}

bool UCalculatePointsWidget::ShouldSkipUpdate() const
{
	return Order == ECalculationOrder::Max || Order == ECalculationOrder::None || CurrentTextBlock == nullptr;
}

void UCalculatePointsWidget::CalculateNumbers(UTextBlock* TextBlockToModify, float StepSize, float EndSize)
{
	if(!TextBlockToModify)
	{
		return;
	}
	
	if(StepSize < 1.0f)
	{
		StepSize = 1.0f;
	}
	for(float i = 0; i <= EndSize; i+=StepSize)
	{
		i = FMath::Clamp(i, 0.0f, EndSize);		
		TextBlockToModify->SetText(FText::AsNumber(FMath::RoundToInt32(i)));
	}
}

void UCalculatePointsWidget::OnTextCommittedCallback(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(!Confirm || Text.IsEmpty())
	{
		return;
	}
	Confirm->SetIsEnabled(true);
}

void UCalculatePointsWidget::OnNameConfirmed()
{
	if(UISubsystem.IsValid() && Name && TotalScore)
	{
		UISubsystem->AddNameToLeaderboardList(Name->GetText().ToString(), TotalPoints);
		TotalPoints = 0.0f;
	}
	if(Switcher && LeaderboardWidget)
	{
		Switcher->SetActiveWidgetIndex(1);
		LeaderboardWidget->Refresh();
	}
}

void UCalculatePointsWidget::CalculateNext()
{
	if(Score == nullptr || PointTimeCurve.GetRichCurveConst() == nullptr || !GetWorld() || !Confirm || !Name || !TimeAmount)
	{
		return;
	}
	Order = static_cast<ECalculationOrder>(FMath::Clamp(static_cast<uint8>(Order) + 1, 0, static_cast<uint8>(ECalculationOrder::Max)));
	switch (Order) {
		case ECalculationOrder::None:
			
			break;
		case ECalculationOrder::Base:
			SetCalculateBase();
			break;
		case ECalculationOrder::Cherries:
			SetCalculateCherries();
			break;
		case ECalculationOrder::Bullet:
			SetCalculateBullet();
			break;
		case ECalculationOrder::Health:
			SetCalculateHealth();
			break;
		case ECalculationOrder::Time:
			SetCalculateTime();
			break;
		case ECalculationOrder::Total:
			SetCalculateTotal();
			break;
		case ECalculationOrder::Max:
			Confirm->SetIsEnabled(false);
			Confirm->SetVisibility(ESlateVisibility::Visible);
			Name->SetVisibility(ESlateVisibility::Visible);
			Name->OnTextCommitted.AddDynamic(this, &ThisClass::OnTextCommittedCallback);
			break;
	default: ;
	}
}

void UCalculatePointsWidget::SetCalculateBase()
{
	if(!Score)
	{
		return;
	}
	CurrentTextBlock = ScoreBase;
	CurrentValue = 0.0f;
	CurrentStepSize = Score->GetScore()/Score->Steps;
	CurrentTopEnd = Score->GetScore();

	TotalPoints += CurrentTopEnd;
}

void UCalculatePointsWidget::SetCalculateCherries()
{
	if(!Score || !AmountCherries)
	{
		return;
	}
	AmountCherries->SetText(FText::AsNumber((Score->GetCherries())));
	CurrentTextBlock = Score_Cherries;
	CurrentValue = 0.0f;
	CurrentStepSize = Score->GetCherryScore()/Score->Steps;
	CurrentTopEnd = Score->GetCherryScore();

	TotalPoints += CurrentTopEnd;
}

void UCalculatePointsWidget::SetCalculateBullet()
{
	if(!Score || !AmountBullet)
	{
		return;
	}
	AmountBullet->SetText(FText::AsNumber(FMath::RoundToInt32(Score->GetAmmo())));
	
	CurrentTextBlock = ScoreBullet;
	CurrentValue = 0.0f;
	CurrentStepSize = Score->GetAmmoScore()/Score->Steps;
	CurrentTopEnd = Score->GetAmmoScore();

	TotalPoints += CurrentTopEnd;
}

void UCalculatePointsWidget::SetCalculateHealth()
{
	if(!Score || !AmountHealth)
	{
		return;
	}
	FNumberFormattingOptions HealthFormatting;
	HealthFormatting.AlwaysSign = false;
	HealthFormatting.UseGrouping = true;
	HealthFormatting.MinimumFractionalDigits = 0;
	HealthFormatting.MaximumFractionalDigits = 1;
	const FText Health = FText::AsNumber(Score->GetHealth()/2, &HealthFormatting);
	AmountHealth->SetText(Health);
	
	CurrentTextBlock = ScoreHealth;
	CurrentValue = 0.0f;
	CurrentStepSize = Score->GetHealthScore()/Score->Steps;
	CurrentTopEnd = Score->GetHealthScore();

	TotalPoints += CurrentTopEnd;
}

void UCalculatePointsWidget::SetCalculateTime()
{
	if(!Score || !TimeAmount || PointTimeCurve.GetRichCurveConst() == nullptr || !UISubsystem.IsValid())
	{
		return;
	}
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
	TimeAmount->SetText(FText::Format(TimeText, Args));
	const float TotalTimePoints = PointTimeCurve.GetRichCurveConst()->Eval(Time.GetTotalSeconds());
	
	CurrentTextBlock = ScoreTime;
	CurrentValue = 0.0f;
	CurrentStepSize = TotalTimePoints/Score->Steps;
	CurrentTopEnd = TotalTimePoints;

	TotalPoints += CurrentTopEnd;

	UISubsystem->ResetTimeDelay();
}

void UCalculatePointsWidget::SetCalculateTotal()
{
	if(!Score)
	{
		return;
	}
	CurrentTextBlock = TotalScore;
	CurrentValue = 0.0f;
	CurrentStepSize = TotalPoints/Score->Steps;
	CurrentTopEnd = TotalPoints;
}
