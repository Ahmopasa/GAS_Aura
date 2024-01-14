// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AueaBeamSpell.h"
#include "GameFramework/Character.h"
#include <Kismet/KismetSystemLibrary.h>
#include <AbilitySystem/AueaAbilitySystemLibrary.h>

void UAueaBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;

		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAueaBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAueaBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (auto* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			const auto SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			FHitResult HitResult;
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true
			);

			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}

	if (auto* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathSignatureDelegate().IsAlreadyBound(this, &UAueaBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathSignatureDelegate().AddDynamic(
				this,
				&UAueaBeamSpell::PrimaryTargetDied
			);
		}
	}
}
					 
void UAueaBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	TArray<AActor*> OverlappingActors;
	UAueaAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors, ActorsToIgnore,
		750.f,
		MouseHitActor->GetActorLocation()
	);

	// auto NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	auto NumAdditionalTargets = 5;

	UAueaAbilitySystemLibrary::GetClosestTargets(
		NumAdditionalTargets,
		OverlappingActors,
		OutAdditionalTargets,
		MouseHitActor->GetActorLocation()
	);

	for (auto* Target : OutAdditionalTargets)
	{
		if (auto* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathSignatureDelegate().IsAlreadyBound(this, &UAueaBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathSignatureDelegate().AddDynamic(
					this,
					&UAueaBeamSpell::AdditionalTargetDied
				);
			}
		}
	}
}
