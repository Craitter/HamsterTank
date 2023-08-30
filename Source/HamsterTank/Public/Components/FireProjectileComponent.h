// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/CollectPickupInterface.h"
#include "FireProjectileComponent.generated.h"


class APawn;
class AProjectileBase;

USTRUCT(BlueprintType)
struct FFireProjectileData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Spread")
	bool bApplySpread = false;
	
	UPROPERTY(EditAnywhere, Category = "Spread")
	float YawSpreadDegree = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	bool bApplyCooldown = false;
	
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	bool bRandomizeCooldown = false;
	
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float MinCooldown = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float MaxCooldown = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float FireCooldown = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Speed")
	bool bApplyCustomSpeed = false;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float CustomSpeed = -1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	bool bRandomizeSpeed = false;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float InMinSpeedModifier = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Speed")
	float InMaxSpeedModifier = 1.0f;
	
	
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMaxAmmoChanged, int32 /* MaxAmmo*/, int32 /* CurrentAmmo*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAMSTERTANK_API UFireProjectileComponent : public UActorComponent, public ICollectPickupInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFireProjectileComponent();
	
	virtual void InitializeComponent() override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	virtual void OnPickupCollected(const EPickupType& Type, const float& Amount, TWeakObjectPtr<APickupActor> CollectedPickup) override;
	
	// UFUNCTION(BlueprintCallable, Category = "Firing")
	AProjectileBase* TryFireProjectile(APawn* InstigatorPawn, const FFireProjectileData& FireData);

	AProjectileBase* TryFireProjectile(APawn* InstigatorPawn = nullptr);
	
	//Usage is for actors to call in constructor so we can customize the projectile Origin,
	//this component will call it in initializeComponent to find the ProjectileOriginComponent if bAutoFindProjectileOrigin is set to true
	void SetProjectileOrigin(TWeakObjectPtr<USceneComponent> InProjectileOrigin = nullptr);
	
	//Only safe to call during construction, afterwards we need to Use Update MaxAmmo
	void SetMaxAmmo(const int32 NewMaxAmmo, int32 InCurrentAmmo);

	//Sets how much ammo the Actor has at the beginning of the game, purpose is mainly to set it from actors on this component in the constructor
	void SetStartAmmo(int32 NewStartAmmo);
	
	//Safe to call whenever since it fires the event
	UFUNCTION(BlueprintCallable, Category = "FireProjectile|Ammo")
	void UpdateMaxAmmo(int32 NewMaxAmmo) const;

	UFUNCTION(BlueprintCallable, Category = "FireProjectile|Ammo")
	void AddAmmoDelta(int32 DeltaAmmo);

	UFUNCTION(BlueprintCallable, Category = "Firing")
	bool CanFireProjectile() const;

	void SetDefaultFireProjectileData(const FFireProjectileData& NewData);
	
	
	FOnAmmoChanged OnAmmoChanged;

	FOnMaxAmmoChanged OnMaxAmmoChanged;

protected:
	
	TWeakObjectPtr<USceneComponent> ProjectileOrigin = {nullptr};

	UPROPERTY(EditAnywhere, Category = "FireProjectile|Origin")
	bool bAutoFindProjectileOrigin;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile|Projectile")
	TSubclassOf<AProjectileBase> BaseProjectile = {nullptr};
	
	UPROPERTY(EditAnywhere, Category = "FireProjectile|Ammo")
	int32 MaxAmmo = 99.0f;

	UPROPERTY(EditAnywhere, Category = "FireProjectile|Ammo")
	int32 StartAmmo = 30.0f;

	UPROPERTY(EditAnywhere, Category = "FireProjectile|Ammo")
	bool bHasEndlessAmmo = false;
	

	UPROPERTY(EditAnywhere, Category = "FireProjectile|Data")
	FFireProjectileData DefaultFireProjectileData;
	
private:
	int32 CurrentAmmo = 0.0f;

	FTimerHandle FireCooldownTimerHandle;
	
public: //Simple Getters

	UFUNCTION(BlueprintCallable, Category = "FireProjectile|Ammo")
	bool GetHasEndlessAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FireProjectile|Ammo")
	int32 GetMaxAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FireProjectile|Ammo")
	int32 GetCurrentAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "FireProjectile|Data")
	void GetDefaultProjectileData(FFireProjectileData& FireProjectileData);
};
