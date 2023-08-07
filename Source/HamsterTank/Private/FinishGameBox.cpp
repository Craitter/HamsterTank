// Fill out your copyright notice in the Description page of Project Settings.


#include "FinishGameBox.h"

#include "ObjectiveSubsystem.h"
#include "Components/ShapeComponent.h"

AFinishGameBox::AFinishGameBox()
{
	if(GetCollisionComponent() != nullptr)
	{
		GetCollisionComponent()->SetCollisionProfileName("OverlapOnlyPawn");
	}
}

void AFinishGameBox::BeginPlay()
{
	Super::BeginPlay();

	if(GetCollisionComponent() != nullptr)
	{
		GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	}
}

void AFinishGameBox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(GetCollisionComponent() != nullptr)
	{
		GetCollisionComponent()->OnComponentBeginOverlap.Clear();
	}
	
	Super::EndPlay(EndPlayReason);
}

void AFinishGameBox::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const TWeakObjectPtr<APawn> OverlappedPawn = Cast<APawn>(OtherActor);
	if(OverlappedPawn.IsValid() && OverlappedPawn->IsPlayerControlled())
	{
		const TWeakObjectPtr<UGameInstance> GameInstance = GetGameInstance();
		if(GameInstance.IsValid())
		{
			TWeakObjectPtr<UObjectiveSubsystem> ObjectiveSubsystem = GameInstance->GetSubsystem<UObjectiveSubsystem>();
			if(ObjectiveSubsystem.IsValid())
			{
				ObjectiveSubsystem->EndGame(OverlappedPawn->GetController());
			}
		}
	}
}
