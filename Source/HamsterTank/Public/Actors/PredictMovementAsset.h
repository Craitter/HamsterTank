// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PredictMovementAsset.generated.h"

/**
 * 
 */
USTRUCT()
struct FPredictedMovement
{
	GENERATED_BODY()
		
	FVector UnRotatedLocation;
	
	FVector RotatedLocation;
};


UCLASS()
class HAMSTERTANK_API UPredictMovementAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FVector GetPredictedMovementLocation(TWeakObjectPtr<APawn> TargetPawn, float SecondsToPredict,
	                                     bool bPredictRotation);

protected:
	

	void RefreshPredictedMovement(TWeakObjectPtr<APawn> TargetPawn, float SecondsToPredict);

	FVector GetPredictedLocation(TWeakObjectPtr<APawn> TargetPawn, float SecondsInFuture, bool bPredictRotation);
	UPROPERTY(EditAnywhere)
	float MaxUpdateTimeBetweenRequests = 0.0f;

	UPROPERTY(EditAnywhere)
	float MaxPredictionTime = 2.0f;
	
	UPROPERTY(EditAnywhere)
	float TimePerStep = 0.1f;
private:
	UPROPERTY(Transient)
	float LastUpdateTimeInSeconds = 0.0f;
	
	TMap<TWeakObjectPtr<APawn>, TArray<FPredictedMovement>> PawnMap;
};
