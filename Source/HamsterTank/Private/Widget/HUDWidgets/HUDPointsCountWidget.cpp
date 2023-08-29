// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUDWidgets/HUDPointsCountWidget.h"

#include "ObjectiveSubsystem.h"
#include "Components/TextBlock.h"


bool UHUDPointsCountWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	if(GetWorld())
	{
		const TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
		if(ObjectiveSubsystem.IsValid())
		{
			TMulticastDelegate<void(float)>* OnScoreChangedDelegate = ObjectiveSubsystem->GetOnScoreChangedDelegate(GetOwningPlayer());
			if(OnScoreChangedDelegate != nullptr)
			{
				OnScoreChangedDelegate->AddUObject(this, &ThisClass::OnScoreChanged);
				const FObjectiveScore* Score = ObjectiveSubsystem->GetScore(GetOwningPlayer());
				if(Score != nullptr && InGameScore != nullptr)
				{
					CurrentScore = Score->GetScore();
					InGameScore->SetText(FText::AsNumber(CurrentScore));
				}
			}
		}
	}

	return true;
}

void UHUDPointsCountWidget::OnScoreChanged(float NewScore)
{
	check(GetWorld())
	TargetScore = NewScore;
	if(!GetWorld()->GetTimerManager().IsTimerActive(ScoreIncreaseHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(ScoreIncreaseHandle, this, &ThisClass::UpdateScore, ScoreIncreaseTimeSteps, true);
	}
}

void UHUDPointsCountWidget::UpdateScore()
{
	check(GetWorld())
	if(InGameScore != nullptr)
	{
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
