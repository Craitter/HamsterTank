// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "TanksterPlayerState.generated.h"

class UAmmoAttributeSet;
class UTanksterAbilitySystemComponent;
class UHealthAttributeSet;
struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API ATanksterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ATanksterPlayerState();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthAttributeSet> HealthAttributeSet = {nullptr};
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAmmoAttributeSet> AmmoAttributeSet = {nullptr};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTanksterAbilitySystemComponent> TanksterAbilitySystem = {nullptr};
	
};
