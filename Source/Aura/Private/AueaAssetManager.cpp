#include "AueaAssetManager.h"
#include "AueaGameplayTags.h"

UAueaAssetManager& UAueaAssetManager::Get()
{
	check(GEngine);
	return *Cast<UAueaAssetManager>(GEngine->AssetManager);
}

void UAueaAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAueaGameplayTags::InitializeNativeGameplayTags();
}
