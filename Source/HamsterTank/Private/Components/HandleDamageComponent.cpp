// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HandleDamageComponent.h"

// Sets default values for this component's properties
UHandleDamageComponent::UHandleDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHandleDamageComponent::BeginPlay()
{
	Super::BeginPlay();
	const TWeakObjectPtr<AActor> Owner = GetOwner();
	if(Owner.IsValid())
	{
		Owner->OnTakePointDamage.AddDynamic(this, &ThisClass::OnOwnerTakePointDamage);
	}
	// ...
	
}

void UHandleDamageComponent::OnOwnerTakePointDamage(AActor* DamagedActor, float Damage,
	AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent,
	FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	const TWeakObjectPtr<AActor> Owner = GetOwner();
	

	
	const FVector ForwardVector = Owner->GetActorForwardVector();
	const float DeltaFwd = ForwardVector.Dot(ShotFromDirection);
	const float Degree = FMath::RadiansToDegrees(acos(abs(DeltaFwd)));
	
	LastHitDirection = ForwardVector.Rotation().GetInverse().RotateVector(ShotFromDirection);
	LastHitDirection *= -1.0f;
	

	if(Degree < 45.0f)
	{
		if(DeltaFwd > UE_FLOAT_NORMAL_THRESH)
		{
			
			if(bModifyDamageOnFront)
			{
				Damage *= MultiplierFrontHit;
			}
		}
		else
		{
			if(bModifyDamageOnBack)
			{
				Damage *= MultiplierBackHit;
			}
		}
	}
	else
	{
		const FVector RightVector = Owner->GetActorRightVector();
		const float DeltaRight = RightVector.Dot(ShotFromDirection);
		if(DeltaRight > UE_FLOAT_NORMAL_THRESH)
		{
			if(bModifyDamageOnRight)
			{
				Damage *= MultiplierRightHit;
			}
		}
		else
		{
			if(bModifyDamageOnLeft)
			{
				Damage *= MultiplierLeftHit;
			}
		}
	}
}

FVector UHandleDamageComponent::GetLastHitDirection()
{
	const FVector OutDirection = LastHitDirection;
	LastHitDirection = FVector::ZeroVector;
	return OutDirection;
}





