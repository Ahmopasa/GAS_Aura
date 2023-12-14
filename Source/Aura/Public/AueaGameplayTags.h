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
	TArray<FGameplayTag> DamageTypes;

	/*
	*	Others
	*/

	FGameplayTag Effects_HitReact;

private:
	static FAueaGameplayTags GameplayTags; 
};