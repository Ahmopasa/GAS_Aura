#include "AbilitySystem/Abilities/AueaFireBolt.h"
#include "Aura/Public/AueaGameplayTags.h"
#include <AbilitySystem/AueaAbilitySystemLibrary.h>
#include "Actor/AueaProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAueaFireBolt::GetDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
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
			ScaledDamage, /*Damage Amount*/
			Cooldown, /*Cooldown Duration*/
			Level, /*Level Amount*/
			ManaCost /*Costs of Mana*/
		);
	else
		return GetNextLevelDescription(Level);
}

FString UAueaFireBolt::GetNextLevelDescription(int32 Level)
{
	const auto ScaledDamage = Damage.GetValueAtLevel(Level);
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
		ScaledDamage, /*Damage Amount*/
		Cooldown, /*Cooldown Duration*/
		Level, /*Level Amount*/
		ManaCost /*Costs of Mana*/
	);
}

void UAueaFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const auto bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
		return;

	const auto SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag
	);

	auto Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
		Rotation.Pitch = PitchOverride;

	const auto Forward = Rotation.Vector();

	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());

	auto Rotations = UAueaAbilitySystemLibrary::EvenlySpacedRotators(
		Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectiles
	);
	for (const auto& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		auto* Projectile = GetWorld()->SpawnActorDeferred<AAueaProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;

		Projectile->FinishSpawning(SpawnTransform);
	}



}
