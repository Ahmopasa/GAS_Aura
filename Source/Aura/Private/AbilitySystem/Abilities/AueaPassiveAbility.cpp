// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AueaPassiveAbility.h"
#include <AbilitySystem/AueaAbilitySystemComponent.h>
#include <AbilitySystemBlueprintLibrary.h>

void UAueaPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (auto* AueaASC = Cast<UAueaAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		AueaASC->DeactivatePassiveAbility.AddUObject(this, &UAueaPassiveAbility::ReceiveDeactivate);
	}		
}

void UAueaPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
