// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UISubsystem.h"

#include "LeaderboardSaveGame.h"
#include "TankHamsterGameInstance.h"
#include "HamsterTank/HamsterTank.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/PromptWidget.h"
#include "Widget/TankBaseWidget.h"

void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameInstance = Cast<UTankHamsterGameInstance>(GetGameInstance());

	RestartLevelDelegate.AddUObject(this, &UUISubsystem::ResetTimeDelay);
}

float UUISubsystem::GetSoundPercentage() const
{
	if(GameInstance.IsValid())
	{
		return GameInstance->GetSoundPercentage();
	}
	else
	{
		return 0.0f;
	}
}

TWeakObjectPtr<UUserWidget> UUISubsystem::PauseGame() const
{
	PauseGameDelegate.Broadcast();
	if(ActiveWidget.IsValid())
	{
		return ActiveWidget;
	}
	else
	{
		return nullptr;
	}
}

void UUISubsystem::UnPauseGame() const
{
	
	UnPauseGameDelegate.Broadcast();
}

void UUISubsystem::RestartLevel() const
{
	RestartLevelDelegate.Broadcast();
}

void UUISubsystem::BackToMainMenu() const
{
	if(GameInstance.IsValid())
	{
		GameInstance->OpenMainMenu();
	}
}

float UUISubsystem::GetGameTimeDelay()
{
	return TotalTimeDelay;
}

void UUISubsystem::AddTimeDelay(float DeltaDelay)
{
	TotalTimeDelay += DeltaDelay;
}

void UUISubsystem::ResetTimeDelay()
{
	
}

FOnPlayerPromptChoice* UUISubsystem::CreatePrompt(EPromptType PromptType, EPromptMessageType MessageType,
                                                  bool& WasSuccessful) const
{
	if(ActiveWidget != nullptr)
	{
		const TObjectPtr<UPromptWidget> Prompt = CreateWidget<UPromptWidget>(GetGameInstance(), GenericPromptClass);
		if(Prompt != nullptr)
		{
			FText PromptMessage;
			FText PromptHeader;
			GetPromptMessageByType(MessageType, PromptMessage, PromptHeader);
			ActiveWidget->EnablePrompt(Prompt);
			Prompt->SetPromptType(PromptType);
			Prompt->SetPromptText(PromptMessage, PromptHeader);
			FOnPlayerPromptChoice* Choice = Prompt->GetPromptReplyDelegate();
			if(Choice != nullptr)
			{
				Choice->AddUObject(this, &UUISubsystem::OnPromptChoiceMade);
			}
			WasSuccessful = true;
			return nullptr;
		}
	}
	WasSuccessful = false;
	return nullptr;
}

void UUISubsystem::GetPromptMessageByType(const EPromptMessageType& PromptMessageType, FText& PromptMessage,
	FText& MessageHeader) const
{
	switch (PromptMessageType)
	{
	case EPromptMessageType::QuitGame:
			PromptMessage = QuitGameText;
			MessageHeader =	QuitGameHeader;
		break;
	case EPromptMessageType::QuitMainMenu:
			PromptMessage = QuitMainMenuText;
			MessageHeader =	QuitMainMenuHeader;
		break;
	case EPromptMessageType::RestartLevel:
			PromptMessage = RestartLevelText;
			MessageHeader =	RestartLevelHeader;
		break;
	case EPromptMessageType::PlayerName:
			PromptMessage =	PlayerNameText;
			MessageHeader =	PlayerNameHeader;
		break;
	default: ;
	}
}

void UUISubsystem::OnPromptChoiceMade(EPromptReply Reply) const
{
	
}

void UUISubsystem::GetLeaderboardList(TArray<FLeaderboardEntry>& CurrentLeaderboard)
{
	//todo: load leaderboard

	if(UGameplayStatics::DoesSaveGameExist(DEFAULT_SAVE_SLOT, 0))
	{
		LeaderboardSaveGame = Cast<ULeaderboardSaveGame>(UGameplayStatics::LoadGameFromSlot(DEFAULT_SAVE_SLOT, 0));
	}
	else
	{
		LeaderboardSaveGame = Cast<ULeaderboardSaveGame>(UGameplayStatics::CreateSaveGameObject(ULeaderboardSaveGame::StaticClass()));
		UGameplayStatics::SaveGameToSlot(LeaderboardSaveGame, DEFAULT_SAVE_SLOT, 0);
	}
	if(LeaderboardSaveGame != nullptr)
	{
		CurrentLeaderboard = LeaderboardSaveGame->Leaderboard;
	}
}

void UUISubsystem::AddNameToLeaderboardList(FString Name, float Points)
{
	
	if(UGameplayStatics::DoesSaveGameExist(DEFAULT_SAVE_SLOT, 0))
	{
		LeaderboardSaveGame = Cast<ULeaderboardSaveGame>(UGameplayStatics::LoadGameFromSlot(DEFAULT_SAVE_SLOT, 0));
	}
	else
	{
		LeaderboardSaveGame = Cast<ULeaderboardSaveGame>(UGameplayStatics::CreateSaveGameObject(ULeaderboardSaveGame::StaticClass()));
		UGameplayStatics::SaveGameToSlot(LeaderboardSaveGame, DEFAULT_SAVE_SLOT, 0);
	}
	if(LeaderboardSaveGame != nullptr)
	{
		FLeaderboardEntry Entry;
		Entry.Score = Points;
		Entry.Name = Name;
		LeaderboardSaveGame->Leaderboard.Add(Entry);
		LeaderboardSaveGame->Leaderboard.Sort();
		
		UGameplayStatics::SaveGameToSlot(LeaderboardSaveGame, DEFAULT_SAVE_SLOT, 0);
	}

}

TWeakObjectPtr<UUserWidget> UUISubsystem::OpenWidget(const TSubclassOf<UTankBaseWidget> WidgetToOpen)
{
	ActiveWidget = CreateWidget<UTankBaseWidget>(GetGameInstance(), WidgetToOpen);
	if(ActiveWidget.IsValid())
	{
		ActiveWidget->AddToViewport();
	}
	return ActiveWidget;
}

void UUISubsystem::NotifyPlayerDead()
{
	OnPlayerDeath.Broadcast();
}

void UUISubsystem::NotifyGameWon()
{
	OnGameWon.Broadcast();
}
