#include "AueaAssetManager.h"
#include "AueaGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAueaAssetManager& UAueaAssetManager::Get()
{
	check(GEngine);
	return *Cast<UAueaAssetManager>(GEngine->AssetManager);
}

void UAueaAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAueaGameplayTags::InitializeNativeGameplayTags();

	// This is required to use Target Data.
	UAbilitySystemGlobals::Get().InitGlobalData();
}
