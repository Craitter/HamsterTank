// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AT_LookAtDynamicLocation.h"

#include "Actors/EnemyTower.h"
#include "Actors/TankBase.h"

UAT_LookAtDynamicLocation::UAT_LookAtDynamicLocation()
{
	bTickingTask = true;
	TowerType = ETowerType::Max;
}

UAT_LookAtDynamicLocation* UAT_LookAtDynamicLocation::LookAtDynamicLocation(UGameplayAbility* OwningAbility,
                                                                            const FName TaskInstanceName, const float InTurningDegreePerSecond,
                                                                            USceneComponent* InComponentToRotate, APawn* InTargetPawn, const ETowerType InTowerType, const FRuntimeFloatCurve& InDistanceSpeedCurve)
{
	UAT_LookAtDynamicLocation* NewTask = NewAbilityTask<UAT_LookAtDynamicLocation>(OwningAbility, TaskInstanceName);
	if(NewTask != nullptr)
	{
		NewTask->ComponentToRotate = InComponentToRotate;
		NewTask->TurningDegreePerSecond = InTurningDegreePerSecond;
		NewTask->TargetPawn = Cast<ATankBase>(InTargetPawn);
		NewTask->TowerType = InTowerType;
		NewTask->DistanceSpeedCurve = InDistanceSpeedCurve.GetRichCurveConst();
		
	}
	return NewTask;
}

void UAT_LookAtDynamicLocation::Activate()
{
	Super::Activate();
}

bool UAT_LookAtDynamicLocation::GetTargetLocation(FVector& TargetLocation)
{
	if(TargetPawn.IsValid() && DistanceSpeedCurve != nullptr && ComponentToRotate.IsValid())
	{
		const float DesiredTravelTime = DistanceSpeedCurve->Eval(FVector::Dist2D(ComponentToRotate->GetComponentLocation(), TargetPawn->GetActorLocation()));
		
		if(TowerType == ETowerType::FullyPredicted)
		{
			TargetLocation = TargetPawn->GetPredictedLocation(DesiredTravelTime, true);
		}
		else if(TowerType == ETowerType::LocationPredicted)
		{
			TargetLocation = TargetPawn->GetPredictedLocation(DesiredTravelTime, false);
		}
		else
		{
			TargetLocation = TargetPawn->GetActorLocation();
		}
	}
	else
	{
		EndTask();
		return false; 
	}
	return true;
}

void UAT_LookAtDynamicLocation::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(!GetIsPaused())
	{
		FVector TargetLocation;
		if(!GetTargetLocation(TargetLocation))
		{
			return;
		}

		if(ComponentToRotate.IsValid())
		{
			const FRotator DesiredRotation = (TargetLocation - ComponentToRotate->GetComponentLocation()).Rotation();
		
			const FQuat TargetQuaternion = DesiredRotation.Quaternion();
			const FQuat ShortestRotation = FQuat::FindBetweenVectors(ComponentToRotate->GetForwardVector(), DesiredRotation.Vector());
			const float DeltaDegree = FMath::RadiansToDegrees(ShortestRotation.GetAngle());
			const float AngleRatio = FMath::Clamp( (DeltaTime * TurningDegreePerSecond) / DeltaDegree, 0.0f, 1.0f);
			const FQuat NewRotation = FQuat::Slerp(ComponentToRotate->GetComponentRotation().Quaternion(),TargetQuaternion , AngleRatio);
				
			ComponentToRotate->SetWorldRotation(NewRotation);
			if(!bIsOnCooldown && TargetQuaternion.Equals(NewRotation))
			{
				LookAtTargetReached.Broadcast(FVector::Dist2D(TargetLocation, ComponentToRotate->GetComponentLocation()));
			}
		}
		else
		{
			EndTask();
		}
	}
	
}

void UAT_LookAtDynamicLocation::SetIsOnCooldown(const bool IsCooldownActive)
{
	bIsOnCooldown = IsCooldownActive;
}

bool UAT_LookAtDynamicLocation::GetIsPaused() const
{
	return bIsPaused;
}

void UAT_LookAtDynamicLocation::SetPause(const bool bPause)
{
	bIsPaused = bPause;
}

