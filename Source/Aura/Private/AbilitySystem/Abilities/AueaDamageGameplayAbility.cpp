#include "AbilitySystem/Abilities/AueaDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAueaDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	auto DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const auto ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(), 
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)
	);
}

FDamageEffectParams UAueaDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(
	AActor* TargetActor,
	FVector InRadialDamageOrigin,
	bool bOverrideKnockbackDirection,
	FVector InOverrideKnockbackDirectio,
	bool bOverrideDeathImpulse,
	FVector InOverrideDeathImpulse,
	bool bOverridePitch,
	float InPitchOverride
) const 
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;

	if (IsValid(TargetActor))
	{
		auto Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		if (bOverridePitch) Rotation.Pitch = InPitchOverride;
		const auto ToTarget = Rotation.Vector();

		if (!bOverrideKnockbackDirection) Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;

		if (!bOverrideDeathImpulse) Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
	}


	if (bOverrideKnockbackDirection)
	{
		InOverrideKnockbackDirectio.Normalize();
		Params.KnockbackForce = InOverrideKnockbackDirectio * KnockbackForceMagnitude;
		
		if (bOverridePitch)
		{
			auto KnockbackRotation = InOverrideKnockbackDirectio.Rotation();
			KnockbackRotation.Pitch = InPitchOverride;

			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}

	if (bOverrideDeathImpulse)
	{
		InOverrideDeathImpulse.Normalize();
		Params.DeathImpulse = InOverrideDeathImpulse * DeathImpulseMagnitude;

		if (bOverridePitch)
		{
			auto DeathImpulseRotation = InOverrideDeathImpulse.Rotation();
			DeathImpulseRotation.Pitch = InPitchOverride;

			Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}

	if (bIsRadialDamage) 
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
		Params.RadialDamageOrigin = InRadialDamageOrigin;
	}

	return Params;
}

float UAueaDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAueaDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages)
{
	if (const auto Size = TaggedMontages.Num(); Size > 0)
	{
		const int32 Selection = FMath::RandRange(0, Size - 1);
		return TaggedMontages[Selection];
	}

	return FTaggedMontage();
}

