// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Health/HealthEntryWidget.h"

#include "Components/Image.h"

bool UHealthEntryWidget::ShouldSkipUpdate(const EHealthState NewState) const
{
	return NewState == CurrentHealthState || FullHealthIcon == nullptr || HalfHealthIcon == nullptr;
}

void UHealthEntryWidget::SetHealthState(const EHealthState NewState)
{
	if(ShouldSkipUpdate(NewState))
	{
		return;
	}
	switch (NewState) {
	case EHealthState::Full:
		FullHealthIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		HalfHealthIcon->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EHealthState::Half:
		FullHealthIcon->SetVisibility(ESlateVisibility::Hidden);
		HalfHealthIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case EHealthState::Empty:
		FullHealthIcon->SetVisibility(ESlateVisibility::Hidden);
		HalfHealthIcon->SetVisibility(ESlateVisibility::Hidden);
		break;
	default: ;
	}
	CurrentHealthState = NewState;
}

EHealthState UHealthEntryWidget::GetCurrentHealthState() const
{
	return CurrentHealthState;
}
