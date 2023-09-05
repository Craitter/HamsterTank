// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/TanksterAbilitySet.h"
#include "GameFramework/Actor.h"
#include "EnemyTower.generated.h"


struct FTanksterAbilitySet_GrantedHandles;
class UTanksterAbilitySet;
class UTanksterGameplayEffect;
struct FOnAttributeChangeData;
class UAmmoAttributeSet;
class UHealthAttributeSet;
class UTanksterAbilitySystemComponent;
class UHandleDamageComponent;
class UHealthComponent;
class UCapsuleComponent;
class UProjectileOriginComponent;
class UFireProjectileComponent;

UENUM(BlueprintType)
enum class ETowerFireType : uint8
{	
	OnTarget,

	Predicted,

	Max				UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ETowerType : uint8
{
	//bLoseTargetWhenOutOfFOV = false;
	//bNeedsUnblockedVisionToAimTargetForFiring = true;
	//bCanOnlyTargetLocationsInRotationRange = false;
	//bRestrainFOVToRotationRange = false;
	//bPredictRotatedLocation = false;
	OnTarget,
	
	//bLoseTargetWhenOutOfFOV = true;
	//bNeedsUnblockedVisionToAimTargetForFiring = true;
	//bCanOnlyTargetLocationsInRotationRange = true;
	//bRestrainFOVToRotationRange = true;
	//bPredictRotatedLocation = false;
	OnTargetOnlyRotationRange,

	//bLoseTargetWhenOutOfFOV = true;
	//bNeedsUnblockedVisionToAimTargetForFiring = true;
	//bCanOnlyTargetLocationsInRotationRange = false;
	//bRestrainFOVToRotationRange = false;
	//bPredictRotatedLocation = false;
	OnTargetOnlyFOV,
	
	//bLoseTargetWhenOutOfFOV = false;
	//bNeedsUnblockedVisionToAimTargetForFiring = false;
	//bCanOnlyTargetLocationsInRotationRange = false;
	//bRestrainFOVToRotationRange = false;
	//bPredictRotatedLocation = true;
	FullyPredicted,

	//bLoseTargetWhenOutOfFOV = false;
	//bNeedsUnblockedVisionToAimTargetForFiring = false;
	//bCanOnlyTargetLocationsInRotationRange = false;
	//bRestrainFOVToRotationRange = false;
	//bPredictRotatedLocation = false;
	LocationPredicted,

	Custom,

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
class HAMSTERTANK_API AEnemyTower : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleCollider = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TowerBase = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TowerHead = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> AnimSkeleton = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> DeadMesh = {nullptr};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFireProjectileComponent> FireProjectileComponent = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileOriginComponent> ProjectileOrigin = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UHandleDamageComponent> HandleDamageComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UTanksterAbilitySystemComponent> TanksterAbilitySystem = {nullptr};
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UHealthAttributeSet> HealthAttributeSet = {nullptr};
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UAmmoAttributeSet> AmmoAttributeSet = {nullptr};
	
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
	UPROPERTY(EditAnywhere, Category = "Tower")
	FCollisionProfileName LookForPlayerCollisionProfileName;
	//If LookForPlayerProfile needs blocking hits to work, this will enable them on that trace, it is set false by
	//default because the default profile is a profile that only overlaps with possible targets so we dont need blocks
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting")
	bool bIgnoreBlocksWhenLookForPlayer;
	//The Radius we look for targets and the max distance we track targets
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting", meta = (ClampMin = "0", ClampMax = "10000", Units = "cm"))
	float MaxRange = 428.0f;
	
	UPROPERTY(EditAnywhere, Category = "Tower|Targeting")
	ETowerType TowerType = ETowerType::OnTarget;
	
	//It would make sense to switch this of, but at the same time do we really want to lose targets when the dive by and we do not rotate fast enough?
	//really depends on the desired behaviour and on MaxTurningDegreePerSecondTargeting
	UPROPERTY(EditAnywhere, Category = "Tower|Behaviour", meta = (EditCondition = "TowerType == ETowerType::Custom", EditConditionHides))
	bool bLoseTargetWhenOutOfFOV = false;
	//When we calculate a TargetLocation, do we want to fire anyway of wait until the
	//bullet will defenetely reach the destination, needs to be checked by design when this is right to be true and when it will be false
	UPROPERTY(EditAnywhere, Category = "Tower|Behaviour", meta = (EditCondition = "TowerType == ETowerType::Custom", EditConditionHides))
	bool bNeedsUnblockedVisionToAimTargetForFiring = false;
	//This is useful for balancing a tower and also if a mesh does not allow more rotation because it would look odd...
	UPROPERTY(EditAnywhere, Category = "Tower|Behaviour", meta = (EditCondition = "TowerType == ETowerType::Custom", EditConditionHides))
	bool bCanOnlyTargetLocationsInRotationRange = false;
	//This is extending on bCanOnlyTargetLocationsInRotationRange it will only allow targets to be recognized
	//within the RotationRange Cone
	UPROPERTY(EditAnywhere, Category = "Tower|Behaviour", meta = (EditCondition = "TowerType == ETowerType::Custom", EditConditionHides))
	bool bRestrainFOVToRotationRange = false;
	//If the right and left input of the tank should be included into the calculation, can be useful at a outer corner turret
	UPROPERTY(EditAnywhere, Category = "Tower|Behaviour", meta = (EditCondition = "TowerType == ETowerType::Custom", EditConditionHides))
	bool bPredictRotatedLocation = false;
	//When bUseRandomFireType is true we can set a TowerFireType here, if we set it to none, it will take a random
	//one at first and then stick with it
	UPROPERTY(EditAnywhere, Category = "Tower|Behaviour", meta = (EditCondition = "TowerType == ETowerType::Custom", EditConditionHides))
	ETowerFireType TowerFireType;

	//The Curve will determine the speed the projectile will have when fired depending on Distance to Target(Todo:Define the x and Y axis UPROPERTY)
	UPROPERTY(EditAnywhere, Category = "Tower|Projectile")
	FRuntimeFloatCurve DistanceToTravelTimeCurve;
	
	UPROPERTY(EditAnywhere, Category = "Tower")
	bool bDebug;

	UPROPERTY(EditAnywhere, Category = "Tower")
	TObjectPtr<UAnimationAsset> DeathAnimation = {nullptr};

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTanksterAbilitySet> InitialAbilitySet = {nullptr};

	FTanksterAbilitySet_GrantedHandles GrantedHandles;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsAlive() const;

	void OnDeath(TWeakObjectPtr<AController> DamageInstigator);

	void OnAnimFinished();
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
	virtual void GetFireTargetLocation(const FVector& InTargetLocation, FVector& OutFireTargetLocation, float& OutDesiredProjectileSpeed);


	/**
	 * @brief Helper Function to get the distance to this actor to a location
	 * @param InLocation the location to calculate distance to 
	 * @return Distance 2D
	 */
	float GetDistanceToSelf2D(const FVector& InLocation) const;
	/**
	 * @brief Tries Accessing the FRuntimeFloatCurve to get the Desired TravelTime Depending on Distance, sets InternDesiredProjectileTravelTime
	 */
	float GetDesiredProjectileTravelTime(const FVector& Location) const;

	/**
	 * @brief Helper Function to Slerp the tower to a desired Rotation at a fixed DegreePerSecond (Always Shortest Path) Can Easily cope with Desired Rotation Changes while updating
	 * @param DesiredRotation The Rotation we want to reach
	 * @param DeltaTime FrameDelta
	 * @param DesiredMaxDegreePerSecond The Rate we rotate with
	 * @return true when the DesiredRotation has been Reached
	 */
	bool RotateToDesiredRotationAtDegreeRate(const FRotator& DesiredRotation, const float DeltaTime, const float DesiredMaxDegreePerSecond) const;

protected:
	

	//Begin Health AttributeChangedCallbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	//End Health AttributeChangedCallbacks
	
	//Begin Ammo AttributeChangedCallbacks
	virtual void AmmoChanged(const FOnAttributeChangeData& Data);
	virtual void MaxAmmoChanged(const FOnAttributeChangeData& Data);
	//End Ammo AttributeChangedCallbacks

	//Begin Health AttributeChangedDelegate
	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	//End Health AttributeChangedDelegate

	//Begin Health AttributeChangedDelegate
	FDelegateHandle AmmoChangedDelegateHandle;
	FDelegateHandle MaxAmmoChangedDelegateHandle;
	//End Health AttributeChangedDelegate

public:
	//Begin Health Attribute Getter
	UFUNCTION(BlueprintCallable, Category = "Tankster|AbilitySystem|AttributeSet|Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Tankster|AbilitySystem|AttributeSet|Health")
	float GetMaxHealth() const;
	//End Health Attribute Getter

	//Begin Ammo Attribute Getter
	UFUNCTION(BlueprintCallable, Category = "Tankster|AbilitySystem|AttributeSet|Ammo")
	float GetAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Tankster|AbilitySystem|AttributeSet|Ammo")
	float GetMaxAmmo() const;
	//End Ammo Attribute Getter

	//Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//End IAbilitySystemInterface

	TWeakObjectPtr<UHealthAttributeSet> GetHealthAttributeSet() const;
	TWeakObjectPtr<UAmmoAttributeSet> GetAmmoAttributeSet() const;
private:
	void UpdateTargeting(const float DeltaTime);
	/**
	 * @brief Checks if there is a Player in Field of View and if it is Visible by our Turret, eventually calls IsLocationInFieldOfView and TryTargetLocation
	 * If it find a Player it will execute OnPlayerFoundDelegateHandle which Calls OnPlayerFound by default (Makes one Sphere with R = MaxTrackDistance and iterates every overlap... See LookForPlayerProfile)
	 * Todo: Expand to be more functional and generalized
	 */
	void LookForPlayer();
	bool ShouldRestrainFOVToRotationRange() const;
	/**
	 * @brief Calculates Degree Distance of FWD Vector and then checks if bigger than FOV/2 
	 * @param TargetLocation Location To Check if in FOV
	 * @return weather it is in FOV or not
	 */
	bool IsLocationInFieldOfView(const FVector& TargetLocation) const;
	/**
	 * @brief Simple Line Trace with custom channel
	 * @param TargetLocation Point To Try To Target
	 */
	bool CanTargetLocation(FVector TargetLocation, TWeakObjectPtr<APawn> PawnToIgnore) const;

	/**
	 * @brief checks against predefined parameters, like out of distance, see bLoseTargetWhenOutOfFOV and bNeedsUnblockedPlayerVisionStayInTargetingState for more
	 * @return if we should abort targeting
	 */
	bool VerifyTargetValid() const;

	bool IsTargetAlive() const;
	
	static bool IsTargetAlive(TWeakObjectPtr<APawn> InTargetPawn);
	float GetMaxRange() const;

	bool IsLocationInRange(const FVector& Location) const;

	bool ShouldVerifyFieldOfView() const;
	
	void SetSinStartEndRotation(const FRotator& InStartRotation, const FRotator& InEndRotation);
	float GetIdleRotationRange() const;

	bool IsDirectionInRotationRange(const FVector& Direction) const;
	float GetFOV() const;

	bool IsDirectionInFOV(const FVector& Direction) const;

	bool IsPredictingTower() const;

	bool ShouldPredictRotation() const;

	bool ShouldVerifyTargetAimLocation() const;

	bool ShouldConstrainAimDirectionToRotationRange() const;

#if ENABLE_DRAW_DEBUG && !NO_CVARS
	void DrawTickDebug() const;
#endif
	
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
	TWeakObjectPtr<APawn> TargetPawn = {nullptr};
	
	FRotator RotationRangeUpperBound = FRotator::ZeroRotator;
	FRotator RotationRangeLowerBound = FRotator::ZeroRotator;
	
	float CurrentTurningTime;
	FRotator SinStartRotation;
	FRotator SinEndRotation;
	float SinDeltaDegree = 0.0f;


	//This property has the Z Location from the ProjectileOriginComponent and the X and Y from the Tower Location,
	//so we trace at the right height, this is espacially useful when the rotation tower has its root at 0 instead of the center of the rotating tower
	FVector InternOriginLocation = FVector::ZeroVector;
	//Temp Intern Values

	float FOVRadians = 0.0f;

	float IdleRotationRangeRadians = 0.0f;
};


