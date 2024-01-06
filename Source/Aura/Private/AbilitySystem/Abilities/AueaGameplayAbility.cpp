// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AueaGameplayAbility.h"
#include "AbilitySystem/AueaAttributeSet.h"

FString UAueaGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(
		TEXT("<Default>%s, </><Level>%d</>"), 
		/*%s*/ L"Default Ability Name - UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk UlyaYuruk.",
		/*%d*/ Level
	);
}

FString UAueaGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(
		TEXT("<Default>Next Level: </><Level>%d</>.\n<Default>Causes much more damage.</>"),
		/*%d*/ Level
	);
}

FString UAueaGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(
		TEXT("<Default>The spell is locked until level: %d</>."),
		/*%d*/ Level
	);
}

float UAueaGameplayAbility::GetManaCost(float InLevel) const
{
	auto ManaCost = 0.f;
	if (const auto CostEffect = GetCostGameplayEffect())
		for (auto Mod : CostEffect->Modifiers)
			if (Mod.Attribute == UAueaAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}

	return ManaCost;
}

float UAueaGameplayAbility::GetCooldown(float InLevel) const
{
	auto Cooldown = 0.f;
	if (const auto CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}

	return Cooldown;
}
