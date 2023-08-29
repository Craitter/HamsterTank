// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUDWidgets/HUDCherryCountWidget.h"

#include "ObjectiveSubsystem.h"
#include "Components/TextBlock.h"

bool UHUDCherryCountWidget::Initialize()
{
	if(!Super::Initialize()) return false;

	if(GetWorld())
	{
		const TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GetWorld()->GetSubsystem<UObjectiveSubsystem>();
		if(ObjectiveSubsystem.IsValid())
		{
			FObjectiveScore* Score = ObjectiveSubsystem->GetScore(GetOwningPlayer());
			if(Score != nullptr)
			{
				Score->OnCherryCollectedDelegateHandle.AddUObject(this, &ThisClass::OnCherryCountChanged);
				OnCherryCountChanged(Score->GetCherries());
			}
		}
	}
	return true;
}

void UHUDCherryCountWidget::OnCherryCountChanged(int32 NewCherryCount) const
{
	if(CherryCount != nullptr)
	{
		const FText Count = FText::FromString("{Amount}");
		FFormatNamedArguments Args;
		Args.Add(TEXT("Amount"), NewCherryCount);
		CherryCount->SetText(FText::Format(Count, Args));
	}
}
