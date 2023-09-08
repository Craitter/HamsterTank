// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AT_RotateIdleSin.h"


UAT_RotateIdleSin::UAT_RotateIdleSin()
{
	bTickingTask = true;
}

UAT_RotateIdleSin* UAT_RotateIdleSin::RotateIdleSin(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const float InYawRotationRange, const float InAverageDegreePerSecond,
									USceneComponent* InComponentToRotate, const FRotator InOriginRotation)
{
	UAT_RotateIdleSin* NewTask = NewAbilityTask<UAT_RotateIdleSin>(OwningAbility, TaskInstanceName);
	if(NewTask != nullptr)
	{
		NewTask->YawRotationRange = InYawRotationRange;
		NewTask->AverageDegreePerSecond = InAverageDegreePerSecond;
		NewTask->ComponentToRotate = InComponentToRotate;
		NewTask->OriginRotation = InOriginRotation;
	}
	return NewTask;
}

void UAT_RotateIdleSin::Activate()
{
	Super::Activate();
	
	if(ComponentToRotate.IsValid())
	{

		// ForwardRotation
		RotationRangeUpperBound = FRotator(OriginRotation.Pitch, OriginRotation.Yaw + YawRotationRange * 0.5f, OriginRotation.Roll);
		RotationRangeLowerBound = FRotator(OriginRotation.Pitch, OriginRotation.Yaw - YawRotationRange * 0.5f, OriginRotation.Roll);
		
		SetSinStartEndRotation(ComponentToRotate->GetComponentRotation(), RotationRangeLowerBound);

		OnSinRotationFinishedDelegate.BindUObject(this, &ThisClass::OnSinRotationFinished);
	}
}

void UAT_RotateIdleSin::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if(ComponentToRotate.IsValid())
	{
		CurrentTurningTime += DeltaTime;

		const float TurningTime = SinDeltaDegree / AverageDegreePerSecond;
		const float Alpha = FMath::Clamp<float>(CurrentTurningTime / TurningTime, 0.0f, 1.0f);
		ComponentToRotate->SetWorldRotation(FMath::InterpSinInOut(SinStartRotation, SinEndRotation, Alpha));
	
		if(Alpha + UE_FLOAT_NORMAL_THRESH > 1.0f)
		{
			CurrentTurningTime = 0.0f;
			if(OnSinRotationFinishedDelegate.IsBound())
			{
				OnSinRotationFinishedDelegate.Execute();
			}
		}
	}
}

void UAT_RotateIdleSin::SetSinStartEndRotation(const FRotator& InStartRotation, const FRotator& InEndRotation)
{
	SinStartRotation = InStartRotation;
	SinEndRotation = InEndRotation;
	SinDeltaDegree = FMath::RadiansToDegrees(InStartRotation.Quaternion().AngularDistance(InEndRotation.Quaternion()));
}

void UAT_RotateIdleSin::OnSinRotationFinished()
{
	if(ComponentToRotate.IsValid())
	{
		const FRotator StartRotation = ComponentToRotate->GetComponentRotation();
		const FRotator EndRotation = StartRotation.Equals(RotationRangeUpperBound) ? RotationRangeLowerBound : RotationRangeUpperBound;
		
		SetSinStartEndRotation(StartRotation, EndRotation);
	}
}
