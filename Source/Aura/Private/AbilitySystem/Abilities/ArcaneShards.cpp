// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	if (Level == 1)
		return FString::Printf(
			TEXT(
				// Title
				"<Title>ARCANE SHARDS</>\n"

				// Description
				"<Default>Launches an arcane shard, causes radial arcane damage at the origin and knockbacks the target.</>\n"

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

FString UArcaneShards::GetNextLevelDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	return FString::Printf(
		TEXT(
			// Title
			"<Title>NEXT LEVEL</>\n"

			// Description
			"<Default>Launches arcane shards, causes radial arcane damage at the origin, also summon additional shards and knockbacks the target.</>\n"

			// Details
			"<Small>- Number of Arcane Shards: </>"
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
		FMath::Min(Level, MaxNumShards), /*Number of Projectiles*/
		ScaledDamage, /*Damage Amount*/
		Cooldown, /*Cooldown Duration*/
		Level, /*Level Amount*/
		ManaCost /*Costs of Mana*/
	);
}
