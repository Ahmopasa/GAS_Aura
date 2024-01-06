#include "AbilitySystem/Abilities/AueaFireBolt.h"
#include "Aura/Public/AueaGameplayTags.h"

FString UAueaFireBolt::GetDescription(int32 Level)
{
	const auto Damage = GetDamageByDamageType(Level, FAueaGameplayTags::Get().Damage_Fire);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	if (Level == 1)
		return FString::Printf(
			TEXT(
				// Title
				"<Title>FIRE BOLT</>\n"

				// Description
				"<Default>Launches a bolt of fire, explodes on impact, deals fire damage and has a chance to burn the target.</>\n"

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
			Damage, /*Damage Amount*/
			Cooldown, /*Cooldown Duration*/
			Level, /*Level Amount*/
			ManaCost /*Costs of Mana*/
		);
	else
		return GetNextLevelDescription(Level);
}

FString UAueaFireBolt::GetNextLevelDescription(int32 Level)
{
	const auto Damage = GetDamageByDamageType(Level, FAueaGameplayTags::Get().Damage_Fire);
	const auto ManaCost = FMath::Abs(GetManaCost(Level));
	const auto Cooldown = GetCooldown(Level);

	return FString::Printf(
		TEXT(
			// Title
			"<Title>NEXT LEVEL</>\n"

			// Description
			"<Default>Launches bolts of fire, explodes on impact, deals fire damage and has a chance to burn the target.</>\n"

			// Details
			"<Small>- Bolts of Fire: </>"
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
		FMath::Min(Level, NumProjectiles), /*Number of Projectiles*/
		Damage, /*Damage Amount*/
		Cooldown, /*Cooldown Duration*/
		Level, /*Level Amount*/
		ManaCost /*Costs of Mana*/
	);
}
