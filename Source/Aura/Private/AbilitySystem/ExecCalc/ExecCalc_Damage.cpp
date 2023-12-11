// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AueaGameplayTags.h"
#include <AbilitySystem/Data/CharacterClassInfo.h>
#include <AbilitySystem/AueaAbilitySystemLibrary.h>
#include <Interaction/CombatInterface.h>

struct AueaDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	AueaDamageStatics() 
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAueaAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAueaAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAueaAttributeSet, BlockChance, Target, false);
	}
};

static const AueaDamageStatics& DamageStatics()
{
	static AueaDamageStatics DStatics;

	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
}

void
UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	const auto* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const auto* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	auto* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	auto* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	auto* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	auto* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	const auto& Spec = ExecutionParams.GetOwningSpec();
	const auto* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const auto* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	////~begin Armor 
	//float Armor = 0.f;
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	//Armor = FMath::Max<float>(0.f, Armor);
	//++Armor;
	//const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	////~end Armor

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAueaGameplayTags::Get().Damage);

	// Capture BlockChance on Target, and determine if it is successful. If Blocked, zero the damage.
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters,TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance; 
	Damage = bBlocked ? 0.f : Damage;

	// Capture Armor Penetration, ignores a percentage of the Target's Armor. 
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const UCharacterClassInfo* CharacterClassInfo = UAueaAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	if (CharacterClassInfo)
	{
		const auto* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
		if (SourceCombatInterface)
		{
			const auto ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
			const float EffectiveArmor = TargetArmor *= ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
		
			const auto* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
			if (TargetCombatInterface)
			{
				const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
				Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
			}
		}
	}

	const FGameplayModifierEvaluatedData EvaluatedData(UAueaAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
