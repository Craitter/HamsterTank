// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraMod_TranslucentObstacles.h"

#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"


#if ENABLE_DRAW_DEBUG && !NO_CVARS
static TAutoConsoleVariable<bool> CVarShowAllTank(
	TEXT("Camera.Modifier.Translucent.Debug"),
	false,
	TEXT("Shows Debug Info for Translucent Obstacles"),
	ECVF_Default);
#endif

bool UCameraMod_TranslucentObstacles::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation,
                                                     FRotator& OutDeltaRot)
{
	check(GetWorld());
	
	const bool bOut = Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);
	//reference @see: https://alfredbaudisch.com/blog/gamedev/unreal-engine-ue/unreal-engine-actors-transparent-block-camera-occlusion-see-through/
	//this is not doing anything to the view rotation, it is just the best place to add this functionality (Only Player Character, easy to enable and disable and all values needed are easy to get)

	if(bDisabled || CameraOwner == nullptr || ViewTarget == nullptr)
	{
		return bOut;
	}
	if(ViewTarget != CachedViewTarget.Get())
	{
		CacheViewTarget(ViewTarget);
	}
	const FVector Start = CameraOwner->GetCameraLocation();
	const FVector End = ViewTarget->GetActorLocation();
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic); //Can Add more Objects to look for here
	
	FCollisionQueryParams CollisionQueryParams;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(ViewTarget);
	CollisionQueryParams.AddIgnoredActors(ActorsToIgnore); // Can Add more Actors to ignore here
	
	TArray<FHitResult> OutHits;

	const bool bGotHits = GetWorld()->SweepMultiByObjectType(
		OutHits, Start, End , FQuat::Identity, ObjectQueryParams,
		FCollisionShape::MakeSphere(CollisionSphereRadius),
		CollisionQueryParams);

#if ENABLE_DRAW_DEBUG && !NO_CVARS

	if(bDebug || CVarShowAllTank->GetBool())
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.0f, 0.0f, 4.0f);
	}
#endif
	
	
	if(bGotHits)
	{
		TSet<const AActor*> NewHitActors;
		for (FHitResult Hit : OutHits)
		{
#if ENABLE_DRAW_DEBUG && !NO_CVARS

			if(bDebug || CVarShowAllTank->GetBool())
			{
				DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 8.0f, FColor::Red, false, -1.0f, 0.0f);
			}
#endif
			
			TWeakObjectPtr<AActor> HitActor = Hit.GetActor();
			CameraObstacleHit(HitActor);
			NewHitActors.Add(HitActor.Get());
		}
		for (auto& Elem : CameraBlockingActors)
		{
			if (!NewHitActors.Contains(Elem.Value.Actor.Get()) && Elem.Value.bIsHidden)
			{
				CameraObstacleIsGone(Elem.Value);
			}
		}
	}
	else
	{
		ForceShowAllCameraObstacles();
	}
	
	return bOut;
}

bool UCameraMod_TranslucentObstacles::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	return Super::ModifyCamera(DeltaTime, InOutPOV);
}

void UCameraMod_TranslucentObstacles::DisableModifier(bool bImmediate)
{
	Super::DisableModifier(bImmediate);
	if(bImmediate)
	{
		ForceShowAllCameraObstacles();
	}
}

void UCameraMod_TranslucentObstacles::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);
	OnHideBlockingActorDelegateHandle.BindStatic(OnCameraObstacleBlocksView);
	OnBlockingActorDisappearedDelegateHandle.BindStatic(OnCameraObstacleStopsBlockingView);

	check(GetWorld())

	GetWorld()->GetTimerManager().SetTimer(CleanCachedHitsTimerHandle, this, &ThisClass::SafeClearCachedHits, LoopTimeCacheClear, true);
}

void UCameraMod_TranslucentObstacles::BeginDestroy()
{
	OnHideBlockingActorDelegateHandle.Unbind();
	OnBlockingActorDisappearedDelegateHandle.Unbind();
	Super::BeginDestroy();
}

void UCameraMod_TranslucentObstacles::CacheViewTarget(TWeakObjectPtr<AActor> ViewTarget)
{
	//This is used a bit like begin play because added to camera is called at a lot of different places pre runtime
	//and can cause engine shutdowns with for example GetViewTarget(), probably an engineBug,
	//this way we can ensure a new view target gets checked as well
	CachedViewTarget = ViewTarget;
	if(SphereRadius < 0.0f || bUseCollisionSphereRadiusAnyway)
	{
#if ENABLE_DRAW_DEBUG && !NO_CVARS
		if(bDebug || CVarShowAllTank->GetBool())
		{
			UE_LOG(LogTemp, Warning , TEXT("%s() Sphere Radius < 0 falling back to CollisionSphere Radius of Viewtarget"),*FString(__FUNCTION__));
		}
#endif
		
		const TWeakObjectPtr<USphereComponent> CollisionSphere = Cast<USphereComponent>(ViewTarget->FindComponentByClass(USphereComponent::StaticClass()));
		if(CollisionSphere.IsValid())
		{
			CollisionSphereRadius = CollisionSphere->GetScaledSphereRadius();
		}
		else
		{
			UE_LOG(LogTemp, Error , TEXT("%s() Sphere Component of ViewTarget is null and sphere radius is -1 so we have no sphere trace size"), *FString(__FUNCTION__));
			DisableModifier(true);
		}
	}
	else
	{
		CollisionSphereRadius = SphereRadius;
	}
	const TWeakObjectPtr<USpringArmComponent> SpringArmComponent = Cast<USpringArmComponent>(ViewTarget->FindComponentByClass(USpringArmComponent::StaticClass()));
	if(SpringArmComponent.IsValid() && SpringArmComponent->bDoCollisionTest)
	{
		UE_LOG(LogTemp, Warning , TEXT("%s() This modifier wont work if the targets springarm has bDoCollisionTest Activated"), *FString(__FUNCTION__));
		DisableModifier(true);
	}
	if(TranslucentMaterial == nullptr)
	{
		UE_LOG(LogTemp, Warning , TEXT("%s() TranslucentMaterial of this Modifier is empty, it wont work"), *FString(__FUNCTION__));
		DisableModifier(true);
	}
}

void UCameraMod_TranslucentObstacles::OnCameraObstacleBlocksView(const FTranslucentCameraObstacle& BlockingActor, const TWeakObjectPtr<UMaterialInterface> InTranslucentMaterial)
{
	for(auto& MapMember : BlockingActor.CachedMeshesMaterials)
	{
		for(int32 i = 0; i < MapMember.Value.Num(); i++)
		{
			if(MapMember.Key.IsValid())
			{
				MapMember.Key->SetMaterial(i, InTranslucentMaterial.Get());
			}
		}
	}
}

void UCameraMod_TranslucentObstacles::CameraObstacleHit(const TWeakObjectPtr<AActor> ObstacleActor)
{
	if(!ObstacleActor.IsValid())
	{
		return;
	}
	FTranslucentCameraObstacle& ExistingObstacle = CameraBlockingActors.FindOrAdd(ObstacleActor);

	if(ExistingObstacle.Actor.IsValid())
	{
		if(!ExistingObstacle.bIsHidden)
		{
			ExistingObstacle.bIsHidden = true;
			OnHideBlockingActorDelegateHandle.Execute(ExistingObstacle, TranslucentMaterial);

#if ENABLE_DRAW_DEBUG && !NO_CVARS

			if(bDebug || CVarShowAllTank->GetBool())
			{
				UE_LOG(LogTemp, Warning , TEXT("%s() Name:%s Actor was already listed but not hidden, hiding now"), *FString(__FUNCTION__), *ExistingObstacle.Actor->GetName());
			}
#endif
		}
		else
		{
#if ENABLE_DRAW_DEBUG && !NO_CVARS

			if(bDebug || CVarShowAllTank->GetBool())
			{
				UE_LOG(LogTemp, Warning , TEXT("%s() Name:%s Actor was already listed and hidden, doing nothing"), *FString(__FUNCTION__), *ExistingObstacle.Actor->GetName());

			}
#endif
		}
	}
	else
	{
		ExistingObstacle.Actor = ObstacleActor;
#if ENABLE_DRAW_DEBUG && !NO_CVARS
		if((bDebug || CVarShowAllTank->GetBool()) && ExistingObstacle.Actor.IsValid())
		{
			UE_LOG(LogTemp, Warning , TEXT("%s() Name:%s Actor was not listed, hiding now"), *FString(__FUNCTION__), *ExistingObstacle.Actor->GetName());
		}
#endif
		
		TArray<UStaticMeshComponent*> MeshComponents;
		ObstacleActor->GetComponents<UStaticMeshComponent>(MeshComponents);
		
		for(TWeakObjectPtr<UStaticMeshComponent> Mesh : MeshComponents)
		{
			TArray<TObjectPtr<UMaterialInterface>>& MaterialInterfaces = ExistingObstacle.CachedMeshesMaterials.Add(Mesh);
			MaterialInterfaces = Mesh->GetMaterials();
		}
		ExistingObstacle.bIsHidden = true;
		// CameraBlockingActors.Add(ObstacleActor, ExistingObstacle);
		OnHideBlockingActorDelegateHandle.Execute(ExistingObstacle, TranslucentMaterial);
	}
}

void UCameraMod_TranslucentObstacles::OnCameraObstacleStopsBlockingView(const FTranslucentCameraObstacle& DisappearedActor)
{
	for(auto& MapMember : DisappearedActor.CachedMeshesMaterials)
	{
		for(int32 i = 0; i < MapMember.Value.Num(); i++)
		{
			if(MapMember.Key.IsValid())
			{
				MapMember.Key->SetMaterial(i, MapMember.Value[i]);
			}
		}
	}	
}

void UCameraMod_TranslucentObstacles::CameraObstacleIsGone(FTranslucentCameraObstacle& CameraObstacle)
{
	if(!CameraObstacle.Actor.IsValid())
	{
		CameraBlockingActors.Remove(CameraObstacle.Actor);
	}

	CameraObstacle.bIsHidden = false;
	OnBlockingActorDisappearedDelegateHandle.Execute(CameraObstacle);
}

void UCameraMod_TranslucentObstacles::ForceShowAllCameraObstacles()
{
	for(auto& Obstacle : CameraBlockingActors)
	{
		if(Obstacle.Value.bIsHidden)
		{
			CameraObstacleIsGone(Obstacle.Value);
		}
	}
}

void UCameraMod_TranslucentObstacles::SafeClearCachedHits()
{
	TArray<TWeakObjectPtr<AActor>> KeysToRemove;
	for(const auto& Obstacle : CameraBlockingActors)
	{
		if(!Obstacle.Value.bIsHidden)
		{
			KeysToRemove.Add(Obstacle.Key);
		}
	}
	for (const TWeakObjectPtr<AActor> ToRemove : KeysToRemove)
	{
		CameraBlockingActors.Remove(ToRemove);
	}
}
