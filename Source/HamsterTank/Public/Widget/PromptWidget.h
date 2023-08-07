// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataTypes/PromptTypes.h"
#include "PromptWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;
	
	FOnPlayerPromptChoice* GetPromptReplyDelegate();

	void SetPromptType(EPromptType Type);
	void SetPromptText(const FText& InDisplayText, const FText& PromptHeader) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DisplayText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HeaderText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LeftButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MiddleButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RightButton;	

private:
	FOnPlayerPromptChoice OnPromptReply;	

	EPromptType PromptType;
	UFUNCTION()
	void OnPromptChoiceMade(EPromptReply Reply);
	UFUNCTION()
	void LeftButtonCallback();
	UFUNCTION()
	void MiddleButtonCallback();
	UFUNCTION()
	void RightButtonCallback();


	void CreateConfirmCancelPrompt() const;
	void CreateConfirmCancelWithInputPrompt() const;

	UPROPERTY(EditDefaultsOnly)
	FText ConfirmText;

	UPROPERTY(EditDefaultsOnly)
	FText CancelText;
};
