// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveTower.generated.h"

class UCapsuleComponent;
class UProjectileOriginComponent;
class UFireProjectileComponent;

UENUM(BlueprintType)
enum class ETowerFireType : uint8
{
	None,
	
	OnTarget,

	Predicted,

	Salve,

	Max				UMETA(Hidden),
};

UENUM()
enum class ETowerTargetingState : uint8
{
	Idle,

	Targeting,

	TargetOutOfView,

	TargetLost,

	None,
};

DECLARE_DELEGATE(FOnSinRotationFinishedDelegate);
DECLARE_DELEGATE_OneParam(FOnPlayerFoundDelegate, TWeakObjectPtr<APawn>);

UCLASS()
class HAMSTERTANK_API AObjectiveTower : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectiveTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleCollider = {nullptr};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Base = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Tower = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFireProjectileComponent> FireProjectileComponent = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileOriginComponent> ProjectileOrigin = {nullptr};
	
	//This changes the min and max rotation on idle, ActorForwardVector +- IdleRotationRange/2	
	UPROPERTY(EditAnywhere, Category = "Tower|Idle", meta = (ClampMin = "0", ClampMax = "360.0", Units = "Degrees"))
	float IdleRotationRange = 60.0f;
	//When going Back to Idle, how fast should it be on average
	UPROPERTY(EditAnywhere, Category = "Tower|Idle", meta = (ClampMin = "0", ClampMax = "5000.0", Units = "Degrees"))
	float AverageTurningDegreePerSecondBackToIdle = 30.0f;
	//when rotating idle in IdleRotationRange hwo fast should it rotate on average
	UPROPERTY(EditAnywhere, Category = "Tower|Idle", meta = (ClampMin = "0", ClampMax = "5000.0", Units = "Degrees"))
	float AverageTurningDegreePerSecondIdle = 30.0f;
	//This has 2 purposes, first it makes it look more real, we lost something, but maybe it reappears so I wait here
	//and prevents short out of vision updates to completely lose the player 
	UPROPERTY(EditAnywhere, Category = "Tower|Idle", meta = (ClampMin = "0", ClampMax = "500.0", Units = "Seconds"))
	float TimeBeforeRotatingBackToIdle = 3.0f;
	
	//The FollowUp Speed when the tower has a TargetLocation to rotate to, this has a big impact on the strength of the turret
	//cause when it doesnt reach the target it will never fire
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting", meta = (ClampMin = "0", ClampMax = "5000.0", Units = "Degrees"))
	float MaxTurningDegreePerSecondTargeting = 150.0f;
	//The Actual View Cone (Tower FWD +- FOV/2) something that is inside will be recognizes as target
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting", meta = (ClampMin = "0", ClampMax = "360.0", Units = "Degrees"))
	float FOV = 30.0f;
	//The Collision Profile we use to find Targets in LookForPlayer(), make sure to use something that is only really
	//detecting Targets because we check later on if we can target the target
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting")
	FCollisionProfileName LookForPlayerProfile;
	//If LookForPlayerProfile needs blocking hits to work, this will enable them on that trace, it is set false by
	//default because the default profile is a profile that only overlaps with possible targets so we dont need blocks
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting")
	bool bIgnoreBlocksWhenLookForPlayer;
	//The Radius we look for targets and the max distance we track targets
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting", meta = (ClampMin = "0", ClampMax = "10000", Units = "cm"))
	float MaxTrackDistance = 3000.0f;
	//It would make sense to switch this of, but at the same time do we really want to lose targets when the dive by and we do not rotate fast enough?
	//really depends on the desired behaviour and on MaxTurningDegreePerSecondTargeting
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting|Behavior")
	bool bLoseTargetWhenOutOfFOV = false;
	//When we calculate a TargetLocation, do we want to fire anyway of wait until the
	//bullet will defenetely reach the destination, needs to be checked by design when this is right to be true and when it will be false
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting|Behavior")
	bool bNeedsUnblockedVisionToAimTargetForFiring = false;
	//This should  never be false with bNeedsUnblockedVisionToAimTargetForFiring because then it can easily
	//feel like the tower knows where the player is even though it doesnt, we wont
	//lose targets easily since we have a delay before going back to idle
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting|Behavior")
	bool bNeedsUnblockedPlayerVisionStayInTargetingState = true;
	//This is useful for balancing a tower and also if a mesh does not allow more rotation because it would look odd...
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting|Behavior")
	bool bCanOnlyTargetLocationsInRotationRange = false;
	//This is extending on bCanOnlyTargetLocationsInRotationRange it will only allow targets to be recognized
	//within the RotationRange Cone
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting|Behavior")
	bool bRestrainFOVToRotationRange = false;
	
	//The Curve will determine the speed the projectile will have when fired depending on Distance to Target(Todo:Define the x and Y axis UPROPERTY)
	UPROPERTY(EditAnywhere, Category = "Tower|Projectile")
	FRuntimeFloatCurve DistanceToTravelTimeCurve;
	//if we should determine a random fire type after every shot or if we should stay with one see TowerFireType
	UPROPERTY(EditAnywhere, Category = "Tower|Projectile")
	bool bUseRandomFireType;
	//When bUseRandomFireType is true we can set a TowerFireType here, if we set it to none, it will take a random
	//one at first and then stick with it
	UPROPERTY(EditAnywhere, Category = "Tower|Projectile", meta = (EditCondition = "!bUseRandomFireType"))
	ETowerFireType TowerFireType;
	
	UPROPERTY(EditAnywhere, Category = "Tower")
	bool bDebug;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:

	/**
	* @brief Validates important members for calculations in the whole Tick Function
	* @return weather it should be Skipped or not
	*/
	virtual bool ShouldSkipUpdate();
	/**
	 * @brief Rotates the Turret of The Tower from SinStartRotation to SinEndRotation at AverageDegreePerSecond with InterpSinInOut
	 * @param AverageDegreePerSecond Used To Determine overall time depending on Start and End Rotation
	 * @param DeltaTime used to increase CurrentTurningTime
	 */
	void RotateTowerSin(const float AverageDegreePerSecond, const float DeltaTime);
	/**
	 * @brief Depending On the FireType it will choose to calculate a fitting TargetLocation
	 * @return The TargetLocation
	 */
	virtual FVector GetFireTargetLocation();
	/**
	 * @brief non weighted Randomization for FireTypes
	 * @return A RandomFireType
	 */
	virtual ETowerFireType GetRandomFireType();


	/**
	 * @brief Helper Function to get the distance to this actor to a location
	 * @param InLocation the location to calculate distance to 
	 * @return Distance 2D
	 */
	float GetDistanceToSelf2D(const FVector& InLocation) const;
	/**
	 * @brief Tries Accessing the FRuntimeFloatCurve to get the Desired TravelTime Depending on Distance
	 * @param InDistance The Depending Distance
	 * @return The DesiredTravelTime of the Projectile
	 */
	float GetDesiredTravelTime(float InDistance) const;

	/**
	 * @brief Helper Function to Slerp the tower to a desired Rotation at a fixed DegreePerSecond (Always Shortest Path) Can Easily cope with Desired Rotation Changes while updating
	 * @param DesiredRotation The Rotation we want to reach
	 * @param DeltaTime FrameDelta
	 * @param DesiredMaxDegreePerSecond The Rate we rotate with
	 * @return true when the DesiredRotation has been Reached
	 */
	bool RotateToDesiredRotationAtDegreeRate(const FRotator& DesiredRotation, const float DeltaTime, const float DesiredMaxDegreePerSecond) const;
	
	ETowerFireType InternTowerFireType;
private:
	
	/**
	 * @brief Checks if there is a Player in Field of View and if it is Visible by our Turret, eventually calls IsLocationInFieldOfView and TryTargetLocation
	 * If it find a Player it will execute OnPlayerFoundDelegateHandle which Calls OnPlayerFound by default (Makes one Sphere with R = MaxTrackDistance and iterates every overlap... See LookForPlayerProfile)
	 * Todo: Expand to be more functional and generalized
	 * @param StartLocation The Location from where we StartLooking for the Player (Usually this should be OurLocation but maybe we want it to work different at some point)
	 */
	void LookForPlayer(const FVector& StartLocation);
	/**
	 * @brief Calculates Degree Distance of FWD Vector and then checks if bigger than FOV/2 
	 * @param StartLocation Location where to StartTracking
	 * @param TargetLocation Location To Check if in FOV
	 * @return weather it is in FOV or not
	 */
	bool IsLocationInFieldOfView(const FVector& StartLocation, const FVector& TargetLocation) const;
	/**
	 * @brief Simple Line Trace with custom channel
	 * @param StartLocation Origin
	 * @param TargetLocation Point To Try To Target
	 * @param OutHitResult The Result
	 */
	void TryTargetLocation(const FVector& StartLocation, const FVector& TargetLocation, FHitResult& OutHitResult) const;

	/**
	 * @brief checks against predefined parameters, like out of distance, see bLoseTargetWhenOutOfFOV and bNeedsUnblockedPlayerVisionStayInTargetingState for more
	 * @return if we should abort targeting
	 */
	bool IsStillTargeting() const;
	
	/**
	 * @brief checks against predefined parameters, like bNeedsUnblockedVisionToAimTargetForFiring and bCanOnlyTargetLocationsInRotationRange
	 * @param AimTargetLocation the Location defined by the FireType and GetFireTargetLocation
	 * @return If we can fire on the TargetLocation
	 */
	bool IsAimTargetLocationValid(const FVector& AimTargetLocation) const;

	//State Changes of Targeting State Begin

	//Reacts to GameplayEvents that lead to a new State
	void OnTargetLost();
	//Reacts to GameplayEvents that lead to a new State
	void OnSinRotationFinished();
	//Reacts to GameplayEvents that lead to a new State
	void OnPlayerFound(TWeakObjectPtr<APawn> InPlayerPawn);

	FTimerHandle WaitForTargetReappearHandle;
	FOnSinRotationFinishedDelegate OnSinRotationFinishedDelegateHandle;
	FOnPlayerFoundDelegate OnPlayerFoundDelegateHandle;
	//State Changes of Targeting State End
	ETowerTargetingState TowerTargetingState = ETowerTargetingState::Idle;

	//Temp Intern Values
	TWeakObjectPtr<APawn> PlayerPawn = {nullptr};
	
	FRotator RotationRangeUpperBound = FRotator::ZeroRotator;
	FRotator RotationRangeLowerBound = FRotator::ZeroRotator;
	
	float CurrentTurningTime;
	FRotator SinStartRotation;
	FRotator SinEndRotation;

	//This property has the Z Location from the ProjectileOriginComponent and the X and Y from the Tower Location,
	//so we trace at the right height, this is espacially useful when the rotation tower has its root at 0 instead of the center of the rotating tower
	FVector InternTargetingOriginLocation = FVector::ZeroVector;
	//Temp Intern Values
};
