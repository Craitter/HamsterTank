// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "CameraMod_TranslucentObstacles.generated.h"

/**
 * 
 */

class USpringArmComponent;

USTRUCT()
struct FTranslucentCameraObstacle
{
	GENERATED_BODY()
	
	TWeakObjectPtr<AActor> Actor = nullptr;

	TMap<TWeakObjectPtr<UStaticMeshComponent>, TArray<TObjectPtr<UMaterialInterface>>> CachedMeshesMaterials;
	
	bool bIsHidden = false;
};

DECLARE_DELEGATE_TwoParams(FOnHideBlockingActorDelegate, const FTranslucentCameraObstacle&, const TWeakObjectPtr<UMaterialInterface> InTranslucentMaterial);
DECLARE_DELEGATE_OneParam(FOnBlockingActorDisappearedDelegate, const FTranslucentCameraObstacle&);

UCLASS()
class HAMSTERTANK_API UCameraMod_TranslucentObstacles : public UCameraModifier
{
	GENERATED_BODY()
public:
	//this will only apply after a view target change
	void SetUseCollisionSphereAnyway(bool bAnyway){ bUseCollisionSphereRadiusAnyway = bAnyway;}
	
	virtual bool ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

	// virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
	virtual void DisableModifier(bool bImmediate) override;

	virtual void AddedToCamera(APlayerCameraManager* Camera) override;
	
	virtual void BeginDestroy() override;
protected:

	//The Material to Apply to Obstacles
	UPROPERTY(EditDefaultsOnly, Category = "Transparency")
	TObjectPtr<UMaterialInterface> TranslucentMaterial = {nullptr};

	//if this is -1 the view targets collision sphere will be used, you can set bUseCollisionSphereAnyway = true, if you want to have both values set and change at runtime
	UPROPERTY(EditAnywhere, Category = "Transparency")
	float SphereRadius = -1.0f;

	//this can override the behavior of which sphere radius will be picked, see @SphereRadius
	UPROPERTY(EditAnywhere, Category = "Transparency")
	bool bUseCollisionSphereRadiusAnyway = false;

	UPROPERTY(EditAnywhere, Category = "Translucence")
	float LoopTimeCacheClear = 5.0f;
	
	FOnHideBlockingActorDelegate OnHideBlockingActorDelegateHandle;
	FOnBlockingActorDisappearedDelegate OnBlockingActorDisappearedDelegateHandle;
private:
	void CacheViewTarget(TWeakObjectPtr<AActor> ViewTarget);

	static void OnCameraObstacleBlocksView(const FTranslucentCameraObstacle& BlockingActor, const TWeakObjectPtr<UMaterialInterface> InTranslucentMaterial);

	void CameraObstacleHit(const TWeakObjectPtr<AActor> ObstacleActor);
	
	static void OnCameraObstacleStopsBlockingView(const FTranslucentCameraObstacle& DisappearedActor);

	void CameraObstacleIsGone(FTranslucentCameraObstacle& CameraObstacle);

	void ForceShowAllCameraObstacles();

	void SafeClearCachedHits();

	TWeakObjectPtr<AActor> CachedViewTarget = {nullptr};
	
	TMap<TWeakObjectPtr<AActor>, FTranslucentCameraObstacle> CameraBlockingActors;

	float CollisionSphereRadius = -1.0f;

	FTimerHandle CleanCachedHitsTimerHandle;
};
