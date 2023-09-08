// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/TanksterAbilitySet.h"
#include "GameFramework/Pawn.h"
#include "TankBase.generated.h"

class UPredictMovementAbility;
enum class ETowerType : uint8;
struct FTanksterAbilitySet_GrantedHandles;
class UTanksterAbilitySet;
struct FGameplayAbilitySpecHandle;
class UTanksterGameplayAbility;
class UTanksterGameplayEffect;
class UAmmoAttributeSet;
class UHealthAttributeSet;
class UTanksterAbilitySystemComponent;
class UNiagaraComponent;
class UCherryObjectiveComponent;
class UCollectPickupComponent;
struct FPickupData;
class UHandleDamageComponent;
class UProjectileOriginComponent;
class UFireProjectileComponent;
enum class EDrivingState : uint8;
class AProjectileBase;
class USphereComponent;
class UFloatingPawnMovement;
class UTankMovementComponent;
class UBoxComponent;
class UCameraComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitDirection, FVector2D, HitDirection);


UCLASS()
class HAMSTERTANK_API ATankBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATankBase();


	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	
	void ClearVelocity() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Tank|Animation")
	FOnHitDirection OnHitDirection;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// void UpdateTowerRotation(float DeltaTime) const;

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere = {nullptr};
	
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Body = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Head = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent = {nullptr};
	
	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UTankMovementComponent> TankMovement = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UProjectileOriginComponent> ProjectileOriginComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UHandleDamageComponent> HandleDamageComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UCherryObjectiveComponent> CherryObjectiveComponent = {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Fire")
	TObjectPtr<UAnimMontage> FireMontage = {nullptr};


public:	

	void OnActorDied(TWeakObjectPtr<AController> DamageInstigator);

	void SetMovementPredictionAbility(const TWeakObjectPtr<UPredictMovementAbility> InAbility, bool bRotated);
private:
	//InternValue
	// FVector DesiredTowerAimLocation = FVector::ZeroVector;
	TWeakObjectPtr<UTanksterAbilitySystemComponent> TanksterAbilitySystem = {nullptr};
	TWeakObjectPtr<UHealthAttributeSet> HealthAttributeSet = {nullptr};
	TWeakObjectPtr<UAmmoAttributeSet> AmmoAttributeSet = {nullptr};

	TWeakObjectPtr<UPredictMovementAbility> PredictRotatedLocationAbility = {nullptr};
	TWeakObjectPtr<UPredictMovementAbility> PredictLocationAbility = {nullptr};
	

public: //simple Getter Functions
	TWeakObjectPtr<USphereComponent> GetSphere() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetBase() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetTower() const;

	TWeakObjectPtr<USpringArmComponent> GetSpringArm() const;

	TWeakObjectPtr<UCameraComponent> GetCamera() const;

	TWeakObjectPtr<UTankMovementComponent> GetTankMovement() const;
	
	FVector GetPredictedLocation(const float& Time, bool bRotated) const;

	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	EDrivingState GetCurrentDrivingState() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	FVector GetVelocityDirection() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	float GetCurrentSpeed() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	float GetCurrentMaxSpeed() const;
	//returns -1 if we are not Sliding;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	float GetSlideDegree() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Movement")
	bool GetIsSliding() const;
	
	


	UFUNCTION(BlueprintCallable, Category = "Tank|Body")
	FString GetBodyName() const;
	
	UFUNCTION(BlueprintCallable, Category = "Tank|Health")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Tank|Damage")
	FVector GetLastHitDirection() const;
	// FVector GetDesiredTargetRotation() const;


	//Begin AbilitySystemInterface
	TWeakObjectPtr<UTanksterAbilitySystemComponent> GetTanksterAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//End AbilitySystemInterface

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTanksterAbilitySet> InitialAbilitySet = {nullptr};
	UPROPERTY(Transient)
	FTanksterAbilitySet_GrantedHandles GrantedHandles;
	
	

	//Begin Health Attribute Getter
	UFUNCTION(BlueprintCallable, Category = "Protelum|AbilitySystem|AttributeSet|Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Protelum|AbilitySystem|AttributeSet|Health")
	float GetMaxHealth() const;
	//End Health Attribute Getter

	//Begin Ammo Attribute Getter
	UFUNCTION(BlueprintCallable, Category = "Tankster|AbilitySystem|AttributeSet|Ammo")
	float GetAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Tankster|AbilitySystem|AttributeSet|Ammo")
	float GetMaxAmmo() const;
	//End Ammo Attribute Getter
};
