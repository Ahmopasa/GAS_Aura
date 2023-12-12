#include "AbilitySystem/AueaAbilitySystemGlobals.h"
#include "AueaAbilityTypes.h"

FGameplayEffectContext* UAueaAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAueaGameplayEffectContext();
}
