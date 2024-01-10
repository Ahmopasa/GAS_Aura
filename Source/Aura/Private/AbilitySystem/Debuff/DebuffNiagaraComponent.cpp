// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include <AbilitySystemBlueprintLibrary.h>
#include <Interaction/CombatInterface.h>
#include "AbilitySystem/AueaAbilitySystemComponent.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* CombatInterface = Cast<ICombatInterface>(GetOwner());

	if (auto* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		ASC->RegisterGameplayTagEvent(
			DebuffTag,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
	else if (CombatInterface)
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(
			this,
			[this](UAbilitySystemComponent* InASC)
			{
				InASC->RegisterGameplayTagEvent(
					DebuffTag,
					EGameplayTagEventType::NewOrRemoved
				).AddUObject(this, &UDebuffNiagaraComponent::DebuffTagChanged);
			}
		);
		
	if (CombatInterface)
		CombatInterface->GetOnDeathDelegate()->AddDynamic(
			this, 
			&UDebuffNiagaraComponent::OnOwnerDeath
		);

}

void UDebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) Activate();
	else Deactivate();
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
