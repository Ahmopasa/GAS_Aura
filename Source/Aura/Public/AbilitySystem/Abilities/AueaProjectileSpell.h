#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AueaGameplayAbility.h"
#include "AueaProjectileSpell.generated.h"

class AAueaProjectile;

UCLASS()
class AURA_API UAueaProjectileSpell : public UAueaGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAueaProjectile> ProjectileClass;
};
