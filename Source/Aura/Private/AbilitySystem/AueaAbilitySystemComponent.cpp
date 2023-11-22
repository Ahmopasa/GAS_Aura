#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AueaGameplayTags.h"
void UAueaAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAueaAbilitySystemComponent::EffectApplied);

	const FAueaGameplayTags& GameplayTags = FAueaGameplayTags::Get();
}

void UAueaAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer; 
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
