// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/CollectPickupInterface.h"
#include "HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnMaxHealthChangedDelegate, float /* NewMaxHealth */, float /* OldMaxHealth */, float /* CurrentHealth */)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeathChangedDelegate, float /* NewHealth */)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeathDelegate, TWeakObjectPtr<AController> /*Instigator*/)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HAMSTERTANK_API UHealthComponent : public UActorComponent, public ICollectPickupInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	void Heal(float Amount);
	void ReceiveFinalDamage(float FinalDamage);
	void ReceiveFinalDamage(float FinalDamage, TWeakObjectPtr<AController> Instigator);
	
	bool IsAlive() const;
	float GetCurrentHealth() const;
	float GetMaxHealth() const;
	void SetMaxHealth(float NewMaxHealth);
	void UpdateMaxHealth(float NewMaxHealth);

	bool IsImmortal() const;

	FOnHeathChangedDelegate OnHeathChangedDelegateHandle;
	FOnDeathDelegate OnDeathDelegateHandle;
	FOnMaxHealthChangedDelegate OnMaxHealthChangedDelegateHandle;

	virtual void OnPickupCollected(const EPickupType& Type, const float& Amount) override;
protected:

	UPROPERTY(EditAnywhere, Category = "Health")
	float MaxHealth = 2;
	
	UPROPERTY(EditAnywhere, Category = "Health")
	bool bIsImmortal = false;
private:
	float CurrentHealth;
};
