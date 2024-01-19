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
	*	Meta Attributes | Incoming XP
	*/

	FGameplayTag Attributes_Meta_IncomingXP;

	/*
	*	Input Tags
	*/

	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

	/*
	*	Input Tags | Block
	*/

	FGameplayTag InputTag_Block_InputPressed;
	FGameplayTag InputTag_Block_InputHeld;
	FGameplayTag InputTag_Block_InputReleased;
	FGameplayTag InputTag_Block_CursorTrace;

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
	*	Debuffs
	*/

	FGameplayTag Debuff_Burn; 
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_ARCANE_WIP;
	FGameplayTag Debuff_PHYSICAL_WIP;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

	/*
	*	Debuffs | Properties
	*/

	FGameplayTag Debuff_Properties_Chance;
	FGameplayTag Debuff_Properties_Damage;
	FGameplayTag Debuff_Properties_Frequency;
	FGameplayTag Debuff_Properties_Duration;

	/*
	*	Abilities
	*/

	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;
	FGameplayTag Abilities_HitReact;
	FGameplayTag Abilities_NONE;

	/*
	*	Abilities | Status
	*/

	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;

	/*
	*	Abilities | Type
	*/

	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_NONE;

	/*
	*	Abilities | Fire 
	*/

	FGameplayTag Abilities_Fire_Firebolt;

	/*
	*	Abilities | Lighting
	*/

	FGameplayTag Abilities_Lighting_LightingBolt;

	/*
	*	Abilities | Arcane
	*/

	FGameplayTag Abilities_Arcane_ArcaneShards;

	/*
	*	Abilities | Passive
	*/

	FGameplayTag Abilities_Passive_HaloOfProtection;
	FGameplayTag Abilities_Passive_LifeSiphon;
	FGameplayTag Abilities_Passive_ManaSiphon;
	
	/*
	*	Cooldown | Fire
	*/

	FGameplayTag Cooldown_Fire_Firebolt;

	/*
	*	Combat Socket
	*/

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	/*
	*	Montage
	*/

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	/*
	*	Effects
	*/

	FGameplayTag Effects_HitReact;

private:
	static FAueaGameplayTags GameplayTags; 
};