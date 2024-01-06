#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AueaGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AueaDamageGameplayAbility.generated.h"


UCLASS()
class AURA_API UAueaDamageGameplayAbility : public UAueaGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages);

	float GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType);
};
