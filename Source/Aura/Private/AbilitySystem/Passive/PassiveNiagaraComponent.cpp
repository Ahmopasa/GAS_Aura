// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include <AbilitySystem/AueaAbilitySystemComponent.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <Interaction/CombatInterface.h>
#include "AueaGameplayTags.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto* AueaASC = Cast<UAueaAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AueaASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
		
		ActivateIfEquipped(AueaASC);
	}
	else if (auto* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddLambda(
			[this](UAbilitySystemComponent* ASC) {
				if (auto* AueaASC = Cast<UAueaAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
				{
					AueaASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
					
					ActivateIfEquipped(AueaASC);
				}
			}
		);
	}
}

void UPassiveNiagaraComponent::ActivateIfEquipped(UAueaAbilitySystemComponent* AueaASC)
{
	const auto bStartupAbilitiesGiven = AueaASC->bStartupAbilitiesGiven;
	if (bStartupAbilitiesGiven)
	{
		if (AueaASC->GetStatusFromAbilityTag(PassiveSpellTag) == FAueaGameplayTags::Get().Abilities_Status_Equipped)
		{
			Activate();
		}
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}
