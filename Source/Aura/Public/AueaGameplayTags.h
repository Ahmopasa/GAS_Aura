#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


/*
*	AueaGameplayTags:
*	A Singleton containing native Gameplay Tags. 
*/
struct FAueaGameplayTags
{
public:
	FORCEINLINE static const FAueaGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/*
	*	Primary Attributes
	*/

	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/*
	*	Secondary Attributes
	*/

	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	/*
	*	Secondary Attributes | Resistance Attributes
	*/

	FGameplayTag Attributes_Secondary_Resistance_Fire;
	FGameplayTag Attributes_Secondary_Resistance_Lighting;
	FGameplayTag Attributes_Secondary_Resistance_Arcane;
	FGameplayTag Attributes_Secondary_Resistance_Physical;

	/*
	*	Input Tags
	*/

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	/*
	*	Damage Types
	*/

	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lighting;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;

	/*
	*	Abilities
	*/

	FGameplayTag Abilities_Attack;

	/*
	*	Montage
	*/

	FGameplayTag Montage_Attack_Weapon;
	FGameplayTag Montage_Attack_RightHand;
	FGameplayTag Montage_Attack_LeftHand;

	/*
	*	Effects
	*/

	FGameplayTag Effects_HitReact;



private:
	static FAueaGameplayTags GameplayTags; 
};