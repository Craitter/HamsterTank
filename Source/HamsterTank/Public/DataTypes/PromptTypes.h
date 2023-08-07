// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PromptTypes.generated.h"

/**
 * 
 */
//this is just here to make it easily expandable
UENUM()
enum class EPromptType : uint8
{
	ConfirmCancel,

	EnterNameConfirmCancel,
};


UENUM()
enum class EPromptReply : uint8
{
	None,
	Confirm,
	Cancel,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerPromptChoice, EPromptReply)

UENUM()
enum class EPromptMessageType : uint8
{
	QuitGame,

	QuitMainMenu,

	RestartLevel,

	PlayerName,
};
