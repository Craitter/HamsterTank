// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TankMovementComponent.generated.h"


UENUM(BlueprintType)
enum class EDrivingState : uint8
{
	DS_Idle = 0,

	DS_Neutral = 1,
	
	DS_Forward = 2,

	DS_Backward = 3,

	DS_Breaking = 4,
};
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UTankMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UTankMovementComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//Here we can react to Pawn Bumps, not sure how we will handle it yet
	virtual void NotifyBumpedPawn(APawn* BumpedPawn) override;
	
	//Here we can play cool things like CameraShake or particles or sounds
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;

	//Overriden to multiply by 100 to make cm/s instead of m/s
	virtual void UpdateComponentVelocity() override;

	virtual bool ShouldSkipUpdate(float DeltaTime) const override;
	// virtual bool ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit, const FQuat& NewRotation) override;

	FVector PredictLocationAfterSeconds(const float Seconds, bool bPredictRotation) const;
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//If you move forward and want to transition to the sliding state this should be called... it does not call SlideAlongSurface!!! it just enables velocity adjustment depending on the angle to the Surface
	void StartSliding(const FVector& ImpactNormal);
	
	//Sets bIsSliding to false and Zeros BlockingDirection
	void StopSliding();

	// currently this only defines the rotation when standing with the tank, since it would be weird to not rotate when standing... Would it? Degree/Second
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Steering", meta = (ClampMin="1.0", ClampMax="360.0", UIMin="1.0", UIMax="360.0", ForceUnits="D/s"))
	float MaxFixedTurningDegree = 90.0f;

	//Right now this applies to the tank when it is driving, so we have a constant turning radius, this mirrors a car behavior
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Steering", meta = (ClampMin="1.0", UIMin="1.0", ForceUnits="m"))
	FRuntimeFloatCurve SpeedTurningCurve;
	
	//The Mass is used to calculate the Force depending on the ConstantAcceleration
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Physics", meta = (ClampMin="1.0", UIMin="1.0", ForceUnits="Kg"))
	float Mass = 64000.0f;
	//The Resistance of the ground to the touching wheels, the higher this number is, the more you feel the missing of force when not applying throttle
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Physics", meta = (ClampMin="0.00001", ClampMax="1.0", UIMin="0.00001", UIMax="1.0"))
	float RollingResistanceCoefficient = 0.2f; // Tanks have 0.045 usually
	// this is the Constant Acceleration applied to the Force when moving forward m/(s*s)
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Physics", meta=(ClampMin="1.0", UIMin="1.0", ForceUnits="m/ss"))
	float ForwardConstantAcceleration = 10.0f;
	// this is the Constant Acceleration applied to the Force when moving Backward m/(s*s)
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Physics", meta=(ClampMin="1.0", UIMin="1.0", ForceUnits="m/ss"))
	float BackwardConstantAcceleration = 10.0f;
	// this is the Distance the tank will move before standing on MaxSpeed, it is constant (Ratio = Velocity/MaxSpeed ==== CurrentBreakingDistance = BreakingDistance * Ratio)
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Physics", meta=(ClampMin="1.0", UIMin="1.0", ForceUnits="cm"))
	float BreakingDistance = 1000.0f;

	//this value scales with max speed and breaking distance, it reaches value from 20 or more to 0.0001 depending on that, making the last bit not feel to natural(inresponsive) you can adjust this number
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Physics", meta=(ClampMin="1.0", UIMin="1.0", ForceUnits="m/ss"))
	float MinBreakingDeceleration = 5.0f;

	//The highest possible speed when driving forward Default Value = 60Km/h / 3.6 = 16.66667
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Speed", meta = (ClampMin="1.0", UIMin="1.0", ForceUnits="m/s"))
	float ForwardMaxSpeed = 16.66667f;
	//The highest possible speed when driving backwards Default Value = 35Km/h / 3.6 = 9,72222 m/s
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Speed", meta = (ClampMin="1.0", UIMin="1.0", ForceUnits="m/s"))
	float BackwardMaxSpeed = 9.72222f;
	
	//if you drive with 90° against a wall it would negate the velocity to see@MinVelocityRatioWhenSliding... this is to keep rotation possible, in this rotation tolerance the velocity is kept constant at this rate while velocity towards wall is given, when trying to drive ion the opposite direction velocity towards the wall gets zeroed to prevent weird behavior, if this value is set to 0.9 for example it would already do that when driving to it with estimated 81° calculate it like this: ACos(0.9) = Angle... to see the degree google AngleToDegree
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Speed", meta = (ClampMin="0.1", ClampMax="1.0", UIMin="0.1", UIMax="1.0", ForceUnits="%"))
	float DeltaRatioWallPawnVelocityOverride = 0.8352f;

	// see @DeltaRatioWallPawnVelocityOverride, When Sliding the velocity gets reduced, this is the smallest velocity Percentage that can be reached (Different directions may have different MaxSpeeds and for that different MinVelocity absolute Values)
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Movement|Speed", meta = (ClampMin="0.1", ClampMax="1.0", UIMin="0.1", UIMax="1.0", ForceUnits="%"))
	float MinVelocityRatioWhenSliding = 0.3f;
	
private:
	//Intern Sliding Values
	bool bIsSliding = false;
	FVector BlockedDirection = FVector::ZeroVector;
	
	// bool bPositionCorrected = false;

	// in N temp value get recalculated each frame
	float CurrentDrivingForce = 0.0f;
	// in m/s temp value get recalculated each frame
	float CurrentMaxSpeed = 0.0f;
	
	EDrivingState DrivingState = EDrivingState::DS_Idle;
	
	void DetermineDrivingState();
	//this recalls Determine Driving Speed in one special case, but since we rely on the new Driving State we cannot do it before
	bool IsStillSliding();
	
	//Set Values Depending On DrivingState Begin
	void SetDrivingValuesDependingOnState();
	void SetForwardDrivingValues();
	void SetBackwardDrivingValues();
	void SetBreakingDrivingValues();
	//return value between 0 and 1 clamped min at MinVelocityRatioWhenSliding
	float GetSlideVelocityRatio() const;
	//Set Values Depending on DrivingState End
	

	//Calculate Velocity Begin
	FVector GetAirResistance(const FVector& InVelocity) const;
	FVector GetRollingResistance(const FVector& InVelocity) const;
	float GetMinTurningRadius(float Speed) const;
	void ComputeVelocity(const float InDeltaTime, FVector& InVelocity) const;
	//Calculate Velocity End

	//Adjust VelocityRotation
	void ProcessUserDesiredRotation(float InDeltaTime);
public: //Simple Getters;
	UFUNCTION(BlueprintCallable, Category = "TankMovement")
	EDrivingState GetCurrentDrivingState() const;
	UFUNCTION(BlueprintCallable, Category = "TankMovement")
	FVector GetVelocityDirection() const;
	UFUNCTION(BlueprintCallable, Category = "TankMovement")
	float GetCurrentSpeed() const;
	//in m/s!!!
	UFUNCTION(BlueprintCallable, Category = "TankMovement")
	float GetCurrentMaxSpeed() const;
	//returns -1 if we are not sliding;
	UFUNCTION(BlueprintCallable, Category = "TankMovement")
	float GetSlideDegree() const;
	//Returns bIsSliding
	UFUNCTION(BlueprintCallable, Category = "TankMovement")
	bool GetIsSliding() const;
private: // Debug Callback(s)
#if ENABLE_DRAW_DEBUG
	static void OnToggleAllDebug(IConsoleVariable* ConsoleVariable);

	void DrawTickDebug() const;
	// void OnTogglePlayerDebug(IConsoleVariable* ConsoleVariable);
	//
	// void OnToggleAIDebug(IConsoleVariable* ConsoleVariable);
#endif
	
};
