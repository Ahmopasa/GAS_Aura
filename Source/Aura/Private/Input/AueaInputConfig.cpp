// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AueaInputConfig.h"

const UInputAction* UAueaInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const auto& Action : AbilityInputActions)
		if (Action.InputAction && Action.InputTag == InputTag)
			return Action.InputAction;

	if (bLogNotFound)
		UE_LOG(
			LogTemp, 
			Error, 
			TEXT("Cannot find AbilityInputAction for InputTag [%s], on InputConfig [%s]"), 
			*InputTag.ToString(), 
			*GetNameSafe(this)
		);

	return nullptr;
}
