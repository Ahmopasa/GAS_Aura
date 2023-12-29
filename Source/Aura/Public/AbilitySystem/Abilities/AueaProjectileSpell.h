#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AueaDamageGameplayAbility.h"
#include "AueaProjectileSpell.generated.h"

class AAueaProjectile;
class UGameplayEffect;
struct FGameplayTag;

UCLASS()
class AURA_API UAueaProjectileSpell : public UAueaDamageGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAueaProjectile> ProjectileClass;


};
