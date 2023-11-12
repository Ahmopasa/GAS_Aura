// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AueaPlayerState.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/AueaAttributeSet.h"

AAueaPlayerState::AAueaPlayerState()
{
	NetUpdateFrequency = 100.f; 

	AbilitySystemComponent = CreateDefaultSubobject<UAueaAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAueaAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AAueaPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAueaPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
