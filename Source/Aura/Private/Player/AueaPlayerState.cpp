// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AueaPlayerState.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAueaPlayerState::AAueaPlayerState()
{
	NetUpdateFrequency = 100.f; 

	AbilitySystemComponent = CreateDefaultSubobject<UAueaAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAueaAttributeSet>("AttributeSet");
}

void AAueaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAueaPlayerState, Level);
	DOREPLIFETIME(AAueaPlayerState, XP);
}

UAbilitySystemComponent* AAueaPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAueaPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AAueaPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAueaPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAueaPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAueaPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAueaPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAueaPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChangedDelegate.Broadcast(XP);
}
