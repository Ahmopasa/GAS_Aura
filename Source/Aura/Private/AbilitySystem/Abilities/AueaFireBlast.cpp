// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AueaFireBlast.h"
#include <AbilitySystem/AueaAbilitySystemLibrary.h>
#include <Actor/AueaFireBall.h>

FString UAueaFireBlast::GetDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	if (Level == 1)
		return FString::Printf(
			TEXT(
				// Title
				"<Title>FIRE BLAST</>\n"

				// Description
				"<Default>Launches a fireball in all directions, each coming back and exploding upon return, causing radial fire damage with a chance to burn.</>\n"

				// Details
				"<Small>- Damage: </>"
				"<Damage>%.2f</>\n"

				"<Small>- Cooldown: </>"
				"<Cooldown>%.2f</>\n"

				"<Small>- Level: </>"
				"<Level>%d</>\n"

				"<Small>- Mana Cost: </>"
				"<ManaCost>%.2f</>\n"
			), /*The Text*/
			ScaledDamage, /*Damage Amount*/
			Cooldown, /*Cooldown Duration*/
			Level, /*Level Amount*/
			ManaCost /*Costs of Mana*/
		);
	else
		return GetNextLevelDescription(Level);
}

FString UAueaFireBlast::GetNextLevelDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	return FString::Printf(
		TEXT(
			// Title
			"<Title>NEXT LEVEL</>\n"

			// Description
			"<Default>Launches fireballs in all directions, each coming back and exploding upon return, causing radial fire damage with a chance to burn.</>\n"

			// Details
			"<Small>- Balls of Fire: </>"
			"<Damage>%d</>\n"

			"<Small>- Damage: </>"
			"<Damage>%.2f</>\n"

			"<Small>- Cooldown: </>"
			"<Cooldown>%.2f</>\n"

			"<Small>- Level: </>"
			"<Level>%d</>\n"

			"<Small>- Mana Cost: </>"
			"<ManaCost>%.2f</>\n"
		),	/*The Text*/
		NumFireBalls, /*Number of Fireballs*/
		ScaledDamage, /*Damage Amount*/
		Cooldown, /*Cooldown Duration*/
		Level, /*Level Amount*/
		ManaCost /*Costs of Mana*/
	);
}

TArray<AAueaFireBall*> UAueaFireBlast::SpawnFireBalls()
{
	TArray<AAueaFireBall*> FireBalls;
	const auto Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const auto Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	auto Rotators =	UAueaAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);
	for (const auto& Rotator : Rotators) {
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		auto* FireBall = GetWorld()->SpawnActorDeferred<AAueaFireBall>(
			FireballClass, 
			SpawnTransform, 
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();	
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();

		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FireBalls.Add(FireBall);

		FireBall->FinishSpawning(SpawnTransform);
	}
	return FireBalls;
}
