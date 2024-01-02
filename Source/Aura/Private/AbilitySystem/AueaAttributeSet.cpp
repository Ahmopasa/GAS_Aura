#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "AueaGameplayTags.h"
#include <Interaction/CombatInterface.h>
#include "Interaction/PlayerInterface.h"
#include <Kismet/GameplayStatics.h>
#include <Player/AueaPlayerController.h>
#include <AbilitySystem/AueaAbilitySystemLibrary.h>

UAueaAttributeSet::UAueaAttributeSet()
{
	const FAueaGameplayTags& GameplayTags = FAueaGameplayTags::Get();

	/*
	*	Primary Attributes
	*/
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilinceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	/*
	*	Secondary Attributes
	*/
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);

	/*
	*	Secondary Attributes | Resistance Attributes
	*/
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Resistance_Lighting, GetLightingResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAueaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*
	*	Primary Attributes
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Resilince, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	/*
	*	Secondary Attributes
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	/*
	*	Secondary Attributes | Resistance Attributes
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, LightingResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	/*
	*	Vital Attributes
	*/
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAueaAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UAueaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAueaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props; 
	SetEffectProperties(Data, Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}

				SendXPEvent(Props);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAueaGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			const bool bBlock = UAueaAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bCriticalHit = UAueaAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCriticalHit);
		}
	}

	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const auto LocalIncomingXP = GetIncomingXP();
		SetIncomingXP(0.f);

		if (Props.SourceCharacter->Implements<UPlayerInterface>())
		IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
	}
}

void UAueaAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Strength, OldStrength);
}

void UAueaAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Intelligence, OldIntelligence);
}

void UAueaAttributeSet::OnRep_Resilince(const FGameplayAttributeData& OldResilince) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Resilince, OldResilince);
}

void UAueaAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Vigor, OldVigor);
}

void UAueaAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Armor, OldArmor);
}

void UAueaAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAueaAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, BlockChance, OldBlockChance);
}

void UAueaAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAueaAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAueaAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAueaAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAueaAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAueaAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, MaxHealth, OldMaxHealth);
}

void UAueaAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, MaxMana, OldMaxMana);
}

void UAueaAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, FireResistance, OldFireResistance);
}

void UAueaAttributeSet::OnRep_LightingResistance(const FGameplayAttributeData& OldLightingResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, LightingResistance, OldLightingResistance);
}

void UAueaAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAueaAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAueaAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Health, OldHealth);
}

void UAueaAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAueaAttributeSet, Mana, OldMana);
}

void UAueaAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAueaAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (auto* PC = Cast<AAueaPlayerController>(Props.SourceCharacter->Controller))
		{
 			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}

		if (auto* PC = Cast<AAueaPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bBlockedHit, bCriticalHit);
		}
	}
}

void UAueaAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (auto* CombatInterface = Cast<ICombatInterface>(Props.TargetCharacter))
	{
		const auto TargetLevel = CombatInterface->GetPlayerLevel();
		const auto TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter);
		const auto XPReward = UAueaAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);
		const auto& GameplayTags = FAueaGameplayTags::Get();
		FGameplayEventData PayLoad; PayLoad.EventTag = GameplayTags.Attributes_Meta_IncomingXP; PayLoad.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, PayLoad);
	}
}
