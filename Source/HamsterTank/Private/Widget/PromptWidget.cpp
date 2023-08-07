// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PromptWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

bool UPromptWidget::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(!ensure(LeftButton != nullptr)) return false;
	if(!ensure(MiddleButton != nullptr)) return false;
	if(!ensure(RightButton != nullptr)) return false;
	
	LeftButton->OnClicked.AddDynamic(this, &UPromptWidget::LeftButtonCallback);
	MiddleButton->OnClicked.AddDynamic(this, &UPromptWidget::MiddleButtonCallback);
	RightButton->OnClicked.AddDynamic(this, &UPromptWidget::RightButtonCallback);
	
	OnPromptReply.AddUObject(this, &UPromptWidget::OnPromptChoiceMade);
	
	return true;
}

FOnPlayerPromptChoice* UPromptWidget::GetPromptReplyDelegate()
{
	return &OnPromptReply;
}

void UPromptWidget::SetPromptType(EPromptType Type)
{
	PromptType = Type;
	switch (Type)
	{
	case EPromptType::ConfirmCancel:
		CreateConfirmCancelPrompt();
		break;
	default: ;
	}
}

void UPromptWidget::SetPromptText(const FText& InDisplayText, const FText& PromptHeader) const
{
	if(DisplayText != nullptr)
	{
		DisplayText->SetText(InDisplayText);
	}
	if(HeaderText != nullptr)
	{
		HeaderText->SetText(PromptHeader);
	}
}

void UPromptWidget::OnPromptChoiceMade(EPromptReply Reply)
{
	this->RemoveFromParent();

}

void UPromptWidget::LeftButtonCallback()
{
	EPromptReply PromptReply = EPromptReply::None;
	switch (PromptType)
	{
	case EPromptType::ConfirmCancel:
		// ReSharper disable once CppAssignedValueIsNeverUsed
		PromptReply = EPromptReply::Confirm;
		break;
	default: ;
	}
	OnPromptReply.Broadcast(PromptReply);
}

void UPromptWidget::MiddleButtonCallback()
{
	EPromptReply PromptReply = EPromptReply::None;
	// switch (PromptType)
	// {
	// }
	OnPromptReply.Broadcast(PromptReply);
}

void UPromptWidget::RightButtonCallback()
{
	EPromptReply PromptReply = EPromptReply::None;
	switch (PromptType)
	{
	case EPromptType::ConfirmCancel:
		// ReSharper disable once CppAssignedValueIsNeverUsed
		PromptReply = EPromptReply::Cancel;
		break;
	default: ;
	}
	OnPromptReply.Broadcast(PromptReply);
}

void UPromptWidget::CreateConfirmCancelPrompt() const
{
	if(MiddleButton)
	{
		MiddleButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPromptWidget::CreateConfirmCancelWithInputPrompt() const
{
	if(MiddleButton)
	{
		MiddleButton->SetVisibility(ESlateVisibility::Hidden);
	}
}
