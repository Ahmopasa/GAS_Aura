#include "AueaGameplayTags.h"
#include "GameplayTagsManager.h"

FAueaGameplayTags FAueaGameplayTags::GameplayTags;

void FAueaGameplayTags::InitializeNativeGameplayTags()
{
	/*
	*	Primary Attributes
	*/

	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage.")
	);

	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage.")
	);

	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increases Armor and Armor Penetration.")
	);

	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increases Health.")
	);

	/*
	*	Secondary Attributes
	*/

	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), 
		FString("Affected by Resilience and increases Block Chance.")
	);

	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Affected by Resilience, ignores the percentage of enemy Armor and increases Critical Hit Chance.")
	);

	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to block the incoming damage. Affected by Armor.")
	);

	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Chance to double the damage plus Critical Hit Damage. Affected by Armor Penetration.")
	);

	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage when a Critical Hit is scored. Affected by Armor Penetration.")
	);

	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduces the Critical Hit Chance of the target. Affected by Armor.")
	);

	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every one second. Affected by Vigor.")
	);

	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every one second. Affected by Vigor.")
	);

	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Affected by Vigor.")
	);

	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Affected by Vigor.")
	);

	/*
	*	Secondary Attributes | Resistance Attributes
	*/

	GameplayTags.Attributes_Secondary_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Resistance.Fire"),
		FString("Resistance to Fire Damage")
	);

	GameplayTags.Attributes_Secondary_Resistance_Lighting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Resistance.Lighting"),
		FString("Resistance to Lighting Damage")
	);

	GameplayTags.Attributes_Secondary_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Resistance.Arcane"),
		FString("Resistance to Arcane Damage")
	);

	GameplayTags.Attributes_Secondary_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.Resistance.Physical"),
		FString("Resistance to Physical Damage")
	);

	/*
	*	Meta Attributes | Incoming XP
	*/

	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Incoming XP Meta Attribute")
	);

	/*
	*	Input Tags
	*/

	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.LMB"),
		FString("Input Tag for Left Mouse Button")
	);

	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("Input Tag for Right Mouse Button")
	);

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("Input Tag for 1 key")
	);

	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("Input Tag for 2 key")
	);

	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("Input Tag for 3 key")
	);

	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("Input Tag for 4 key")
	);

	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.1"),
		FString("Input Tag for Passive Ability 1")
	);

	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Passive.2"),
		FString("Input Tag for Passive Ability 2")
	);

	/*
	*	Input Tags | Block
	*/

	GameplayTags.InputTag_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Block.InputPressed"),
		FString("Block InputTag for Press callback")
	);

	GameplayTags.InputTag_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Block.InputHeld"),
		FString("Block InputTag for Held callback")
	);

	GameplayTags.InputTag_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Block.InputReleased"),
		FString("Block InputTag for Release callback")
	);

	GameplayTags.InputTag_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Block.CursorTrace"),
		FString("Block InputTag for CursorTrace callback")
	);
	/*
	*	Damage Types
	*/

	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
	);

	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage Type")
	);

	GameplayTags.Damage_Lighting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Lighting"),
		FString("Lighting Damage Type")
	);

	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage Type")
	);

	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage Type")
	);

	/*
	*	Mapping Damage Types to Resistances
	*/

	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Secondary_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lighting, GameplayTags.Attributes_Secondary_Resistance_Lighting);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Secondary_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Secondary_Resistance_Physical);

	/*
	*	Debuffs
	*/

	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Burn"),
		FString("Debuff of Fire Damage")
	);

	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Stun"),
		FString("Debuff of Lighting Damage")
	);

	GameplayTags.Debuff_ARCANE_WIP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.ARCANE_WIP"),
		FString("Debuff of Arcane Damage")
	);

	GameplayTags.Debuff_PHYSICAL_WIP = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.PHYSICAL_WIP"),
		FString("Debuff of Physical Damage")
	);

	/*
	*	Mapping Damage Types to Debuffs
	*/

	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Fire, GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Lighting, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Arcane, GameplayTags.Debuff_ARCANE_WIP);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_PHYSICAL_WIP);

	/*
	*	Debuffs | Properties
	*/

	GameplayTags.Debuff_Properties_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Properties.Chance"),
		FString("Debuff Chance")
	);

	GameplayTags.Debuff_Properties_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Properties.Damage"),
		FString("Debuff Damage")
	);

	GameplayTags.Debuff_Properties_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Properties.Frequency"),
		FString("Debuff Frequency")
	);

	GameplayTags.Debuff_Properties_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Debuff.Properties.Duration"),
		FString("Debuff Duration")
	);

	/*
	*	Abilities
	*/

	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Attack"),
		FString("Attack Ability Tag")
	);

	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("Summon Ability Tag")
	);

	GameplayTags.Abilities_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("Hit React Ability")
	);

	GameplayTags.Abilities_NONE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.NONE"),
		FString("No Ability Tag - like nullptr")
	);

	/*
	*	Abilities | Status
	*/

	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Locked"),
		FString("Locked Status")
	);

	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Eligible"),
		FString("Eligible Status")
	);

	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Unlocked"),
		FString("Unlocked Status")
	);
	
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Status.Equipped"),
		FString("Equipped Status")
	);

	/*
	*	Abilities | Type
	*/

	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Offensive"),
		FString("Offensive Type")
	);

	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.Passive"),
		FString("Passive Type")
	);

	GameplayTags.Abilities_Type_NONE = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Type.NONE"),
		FString("NONE Type")
	);

	/*
	*	Abilities | Fire
	*/

	GameplayTags.Abilities_Fire_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Fire.Firebolt"),
		FString("Firebolt Ability Tag")
	);

	/*
	*	Abilities | Lighting
	*/

	GameplayTags.Abilities_Lighting_LightingBolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Abilities.Lighting.Lightingbolt"),
		FString("Lightingbolt Ability Tag")
	);

	/*
	*	Cooldown | Fire
	*/

	GameplayTags.Cooldown_Fire_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Cooldown.Fire.Firebolt"),
		FString("Firebolt Cooldown Tag")
	);

	/*
	*	Combat Socket
	*/

	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Weapon"),
		FString("Weapon")
	);

	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.RightHand"),
		FString("Right Hand")
	);

	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.LeftHand"),
		FString("Left Hand")
	);

	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("CombatSocket.Tail"),
		FString("Tail")
	);

	/*
	*	Montage
	*/

	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.1"),
		FString("Attack 1")
	);

	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.2"),
		FString("Attack 2")
	);

	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.3"),
		FString("Attack 3")
	);

	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Montage.Attack.4"),
		FString("Attack 4")
	);

	/*
	*	Effects
	*/

	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("Tag granted when Hit Reacting")
	);
}
