// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_FindPlayer.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindPlayerDelegate, APawn*, FoundPlayerPawn);

UCLASS()
class HAMSTERTANK_API UAT_FindPlayer : public UAbilityTask
{
	GENERATED_BODY()

public:

	UAT_FindPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UAT_FindPlayer* FindPlayer(UGameplayAbility* OwningAbility, const FName TaskInstanceName, const float InTimerRate, const float InMaxRange,
	                                  float InFOVRadians, USceneComponent* InOriginComponent);

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	UPROPERTY(BlueprintAssignable)
	FFindPlayerDelegate PlayerFoundDelegate;

	UPROPERTY(BlueprintAssignable)
	FFindPlayerDelegate PlayerLostDelegate;

	float MaxRange = 0.0f;

	float FOVRadians = 0.0f;

	float TimerRate = 0.0f;

	TWeakObjectPtr<USceneComponent> OriginComponent = {nullptr};
protected:
	
	TWeakObjectPtr<APawn> GetPlayerPawn() const;
	static TWeakObjectPtr<UAbilitySystemComponent> GetTargetASC(TWeakObjectPtr<APawn> InTargetPawn);
private:
	FTimerHandle FindPlayerTimerHandle;
	FCollisionQueryParams QueryParams;
	void FindPlayer();
	bool TryFindPlayer(TWeakObjectPtr<APawn> InTargetPawn, TWeakObjectPtr<UAbilitySystemComponent> InTargetASC) const;
	
	bool IsTargetInRange2D(TWeakObjectPtr<APawn> InTargetPawn) const;
	static bool IsTargetAlive(const TWeakObjectPtr<UAbilitySystemComponent> InTargetASC);
	bool IsTargetInFOV(TWeakObjectPtr<APawn> InTargetPawn) const;
	bool IsVisionToTargetUnblocked(TWeakObjectPtr<APawn> InTargetPawn) const;

	void PlayerFound(TWeakObjectPtr<APawn> InFoundPlayer);
	void PlayerLost();

	bool bPlayerFound = false;
	TWeakObjectPtr<APawn> FoundPlayer = {nullptr};
};
