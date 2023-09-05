// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "TankBase.generated.h"

class UTanksterGameplayEffect;
class UAmmoAttributeSet;
class UHealthAttributeSet;
class UTanksterAbilitySystemComponent;
class UNiagaraComponent;
class UCherryObjectiveComponent;
class UCollectPickupComponent;
struct FPickupData;
class UHandleDamageComponent;
class UHealthComponent;
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
	TObjectPtr<UFireProjectileComponent> FireProjectileComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UProjectileOriginComponent> ProjectileOriginComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UHandleDamageComponent> HandleDamageComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UCollectPickupComponent> CollectPickupComponent = {nullptr};

	UPROPERTY(VisibleAnywhere, Category = "Tank|EssentialComponents", BlueprintReadOnly)
	TObjectPtr<UCherryObjectiveComponent> CherryObjectiveComponent = {nullptr};
	
	UPROPERTY(EditDefaultsOnly, Category = "Tank|Fire")
	TObjectPtr<UAnimMontage> FireMontage = {nullptr};

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UTanksterGameplayEffect>> InitAttributesEffects = {nullptr};

public:	
	// void RequestAimAtTarget(const FVector& TargetLocation);
	void RequestFire();

	void OnActorDied(TWeakObjectPtr<AController> DamageInstigator);

private:
	//InternValue
	// FVector DesiredTowerAimLocation = FVector::ZeroVector;
	TWeakObjectPtr<UTanksterAbilitySystemComponent> TanksterAbilitySystem = {nullptr};
	TWeakObjectPtr<UHealthAttributeSet> HealthAttributeSet = {nullptr};
	TWeakObjectPtr<UAmmoAttributeSet> AmmoAttributeSet = {nullptr};

public: //simple Getter Functions
	TWeakObjectPtr<USphereComponent> GetSphere() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetBase() const;

	TWeakObjectPtr<USkeletalMeshComponent> GetTower() const;

	TWeakObjectPtr<USpringArmComponent> GetSpringArm() const;

	TWeakObjectPtr<UCameraComponent> GetCamera() const;

	TWeakObjectPtr<UTankMovementComponent> GetTankMovement() const;

	TWeakObjectPtr<UFireProjectileComponent> GetFireProjectileComponent() const;

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
	
	
	UFUNCTION(BlueprintCallable, Category = "Tank|Firing")
	bool GetHasEndlessAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Firing")
	int32 GetCurrentAmmo() const;
	UFUNCTION(BlueprintCallable, Category = "Tank|Firing")
	int32 GetMaxAmmoDEPRECATEd() const;

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
