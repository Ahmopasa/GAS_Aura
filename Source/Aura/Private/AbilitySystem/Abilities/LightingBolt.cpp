// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LightingBolt.h"

FString ULightingBolt::GetDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	if (Level == 1)
		return FString::Printf(
			TEXT(
				// Title
				"<Title>LIGHTING BOLT</>\n"

				// Description
				"<Default>Launches beam of lighting, stickts to the target, deals lighting damage repeatedly and has a chance to stun the target.</>\n"

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

FString ULightingBolt::GetNextLevelDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	return FString::Printf(
		TEXT(
			// Title
			"<Title>NEXT LEVEL</>\n"

			// Description
			"<Default>Launches beam of lighting, stickts to the target, deals lighting damage repeatedly, also propagates nearby targets and has a chance to stun the target.</>\n"

			// Details
			"<Small>- Number of Targets: </>"
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
		FMath::Min(Level, MaxNumShockTargets), /*Number of Propagated Targets*/
		ScaledDamage, /*Damage Amount*/
		Cooldown, /*Cooldown Duration*/
		Level, /*Level Amount*/
		ManaCost /*Costs of Mana*/
	);
}
