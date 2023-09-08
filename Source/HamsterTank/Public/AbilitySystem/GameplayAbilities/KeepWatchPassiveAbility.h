// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/TanksterGameplayAbility.h"
#include "KeepWatchPassiveAbility.generated.h"

class UAT_LookAtDynamicLocation;
enum class ETowerType : uint8;
class UAT_FindPlayer;
class AEnemyTower;
class UAT_RotateIdleSin;
/**
 * 
 */
UCLASS()
class HAMSTERTANK_API UKeepWatchPassiveAbility : public UTanksterGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:
	UFUNCTION()
	void OnPlayerFound(APawn* TargetPawn);

	UFUNCTION()
	void OnPlayerLost(APawn* TargetPawn);

	UFUNCTION()
	void OnTargetLocationReached(const float& Distance);

	UFUNCTION()
	void OnFireProjectileCooldownTagChanged(FGameplayTag ChangedTag, int NewCount) const;
	
	void StartIdleRotation(TWeakObjectPtr<AEnemyTower> Tower);
	void StopIdleRotation() const;
	
	void StartKeepWatch(TWeakObjectPtr<AEnemyTower> Tower);

	void ListenCooldownTagChanged();
	void RemoveCooldownTagChangedListener() const;

	void StartLookAtTargetLocation(TWeakObjectPtr<APawn> InTargetPawn);
	void PauseLookAtTargetLocation();
	
	void OnWaitForPlayerEnds();

	

	
	
	TWeakObjectPtr<AEnemyTower> GetOwningTower() const;
	static TWeakObjectPtr<UAbilitySystemComponent> GetTargetAbilitySystemComponent(TWeakObjectPtr<APawn> TargetPawn);
private:
	void GetTowerType(TWeakObjectPtr<AEnemyTower> Tower);


	void NotifyTargetForPrediction(const TWeakObjectPtr<APawn> TargetPawn);
	void NotifyTargetStopPrediction(const TWeakObjectPtr<APawn> TargetPawn) const;
	TObjectPtr<UAT_RotateIdleSin> RotateSinTask = {nullptr};
	TObjectPtr<UAT_FindPlayer> FindPlayerTask = {nullptr};
	TObjectPtr<UAT_LookAtDynamicLocation> LookAtLocationTask = {nullptr};

	ETowerType TowerType;

	FTimerHandle WaitForPlayerReturn;

	FActiveGameplayEffectHandle GETargetPredictsHandle;

	FDelegateHandle CooldownTagChangedHandle;
};
