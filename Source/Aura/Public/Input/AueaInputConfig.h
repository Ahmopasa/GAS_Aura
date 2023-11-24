#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AueaInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FAueaInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

UCLASS()
class AURA_API UAueaInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAueaInputAction> AbilityInputActions;
};
