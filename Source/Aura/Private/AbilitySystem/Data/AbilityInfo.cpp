// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"
#include "AueaLogChannel.h"

FAueaAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const auto& Info : AbilityInformation)
		if (Info.AbilityTag == AbilityTag)
			return Info; 

	if (bLogNotFound)
		UE_LOG(LogAuea, Error, TEXT("Cannot find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));

	return FAueaAbilityInfo();
}
