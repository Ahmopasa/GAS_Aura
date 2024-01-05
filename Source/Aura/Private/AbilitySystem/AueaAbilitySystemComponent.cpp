#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AueaGameplayTags.h"
#include "AbilitySystem/Abilities/AueaGameplayAbility.h"
#include "AueaLogChannel.h"
#include <Interaction/PlayerInterface.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <AbilitySystem/AueaAbilitySystemLibrary.h>
#include "AbilitySystem/Data/AbilityInfo.h"

FGameplayTag UAueaAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
		for (auto Tag : AbilitySpec.Ability.Get()->AbilityTags)
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
				return Tag;

	return FGameplayTag();
}

FGameplayTag UAueaAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (auto Tag : AbilitySpec.DynamicAbilityTags)
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
			return Tag;

	return FGameplayTag();
}

FGameplayTag UAueaAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (auto StatusTag : AbilitySpec.DynamicAbilityTags)
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
			return StatusTag;

	return FGameplayTag();
}

FGameplayAbilitySpec* UAueaAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (auto& AbilitySpec : GetActivatableAbilities())
		for (auto Tag : AbilitySpec.Ability.Get()->AbilityTags)
			if (Tag.MatchesTag(AbilityTag))
				return &AbilitySpec; 

	return nullptr;
}

void UAueaAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAueaAbilitySystemComponent::ClientEffectApplied);

	const FAueaGameplayTags& GameplayTags = FAueaGameplayTags::Get();
}

void UAueaAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const auto& AbilityClass : StartupAbilities)
	{
		auto AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAueaGameplayAbility* AueaAbility = Cast<UAueaGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AueaAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAueaGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}

	bStartupAbilitiesGiven = true;

	AbilitiesGivenDelegate.Broadcast();
}

void UAueaAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (auto AbilityClass : StartupPassiveAbilities)
	{
		auto AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAueaAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);

			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAueaAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAueaAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (const auto& AbilitySpec : GetActivatableAbilities())
		if (!Delegate.ExecuteIfBound(AbilitySpec))
			UE_LOG(LogAuea, Error, TEXT("Failed to execute delegate in %hs."), __FUNCTION__);
}

void UAueaAbilitySystemComponent::UpgradeAttributes(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
			ServerUpgradeAttribute(AttributeTag);
}

void UAueaAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData PayLoad;
	PayLoad.EventTag = AttributeTag;
	PayLoad.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetAvatarActor(), 
		AttributeTag, 
		PayLoad
	);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
}

void UAueaAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
	auto AbilityInfo = UAueaAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const auto& Info : AbilityInfo->AbilityInformation)
		if (Level >= Info.LevelRequirement && Info.AbilityTag.IsValid() && GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			auto AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FAueaGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, FAueaGameplayTags::Get().Abilities_Status_Eligible);
		}
}

void UAueaAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven) 
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAueaAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag);
}

void UAueaAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer; 
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}
