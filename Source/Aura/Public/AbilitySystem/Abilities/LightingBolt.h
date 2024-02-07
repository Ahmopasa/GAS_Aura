#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AueaBeamSpell.h"
#include "LightingBolt.generated.h"

UCLASS()
class AURA_API ULightingBolt : public UAueaBeamSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
