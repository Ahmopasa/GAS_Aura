#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AueaDamageGameplayAbility.h"
#include "ArcaneShards.generated.h"

UCLASS()
class AURA_API UArcaneShards : public UAueaDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNumShards = 11; 
};
