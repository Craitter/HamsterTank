// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DisplayCherryWidget.h"

#include "Components/CherryObjectiveComponent.h"
#include "Components/TextBlock.h"

void UDisplayCherryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	const TWeakObjectPtr<APawn> Pawn = GetOwningPlayerPawn();
	if(Pawn.IsValid())
	{
		const TWeakObjectPtr<UCherryObjectiveComponent> CherryObjectiveComponent = Pawn->FindComponentByClass<UCherryObjectiveComponent>();
		if(CherryObjectiveComponent.IsValid())
		{
			OnCherryCountChanged(CherryObjectiveComponent->GetCurrentCherries());
			CherryObjectiveComponent->OnCherryCountChangedDelegateHandle.AddUObject(this, &ThisClass::OnCherryCountChanged);
		}
	}
}

void UDisplayCherryWidget::OnCherryCountChanged(float NewCherryCount)
{
	if(CherryCount != nullptr)
	{
		const FText Count = FText::FromString("{Amount}");
		FFormatNamedArguments Args;
		Args.Add(TEXT("Amount"), NewCherryCount);
		CherryCount->SetText(FText::Format(Count, Args));
	}
}
