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

FGameplayTag UAueaAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const auto Spec = GetSpecFromAbilityTag(AbilityTag))
		return GetStatusFromSpec(*Spec);

	return FGameplayTag();
}

FGameplayTag UAueaAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const auto Spec = GetSpecFromAbilityTag(AbilityTag))
		return GetInputTagFromSpec(*Spec);

	return FGameplayTag();
}

bool UAueaAbilitySystemComponent::IsSlotEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (auto& AbilitySpec : GetActivatableAbilities())
		if (HasAbilitySlot(AbilitySpec, Slot))
			return false;

	return true;
}

bool UAueaAbilitySystemComponent::HasAbilitySlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	return Spec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UAueaAbilitySystemComponent::HasAnyAbilitySlot(const FGameplayAbilitySpec& Spec)
{
	return Spec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAueaAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);

	for (auto& AbilitySpec : GetActivatableAbilities())
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(Slot))
			return &AbilitySpec;

	return nullptr;
}

bool UAueaAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	const auto* AbilityInfo = UAueaAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const auto AbilityTag = GetAbilityTagFromSpec(Spec);
	const auto& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const auto AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(FAueaGameplayTags::Get().Abilities_Type_Passive);
}

void UAueaAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	ClearSlot(&Spec);

	Spec.DynamicAbilityTags.AddTag(Slot);
}

void UAueaAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
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

void UAueaAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);

			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					AbilitySpec.Handle,
					AbilitySpec.ActivationInfo.GetActivationPredictionKey()
				);
			}
		}
	}
}

void UAueaAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);

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

	FScopedAbilityListLock ActiveScopeLock(*this);

	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				AbilitySpec.Handle,
				AbilitySpec.ActivationInfo.GetActivationPredictionKey()
			);
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
			ClientUpdateAbilityStatus(Info.AbilityTag, FAueaGameplayTags::Get().Abilities_Status_Eligible, 1);
		}
}

void UAueaAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (auto AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		const auto GameplayTags = FAueaGameplayTags::Get();
		auto Status = GetStatusFromSpec(*AbilitySpec);
		if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		else
		{
			// EXCEPTION
		}

		ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAueaAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (auto AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const auto& GameplayTags = FAueaGameplayTags::Get();
		const auto& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const auto& Status = GetStatusFromSpec(*AbilitySpec);
		const auto bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// Handle the activation/deactivation for passive abilities:
			if (!IsSlotEmpty(Slot))
			{
				auto* SpecWithSlot = GetSpecWithSlot(Slot);
				if (SpecWithSlot)
				{
					// Check if that ability same as this ability:  
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
						return; 
					}

					if (IsPassiveAbility(*SpecWithSlot))
					{
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}

					ClearSlot(SpecWithSlot);
				}
			}

			// Check if the ability is active or not: 
			if (!HasAnyAbilitySlot(*AbilitySpec))
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
			}

			AssignSlotToAbility(*AbilitySpec, Slot);

			MarkAbilitySpecDirty(*AbilitySpec);
			// DO NO DELETE YET
			//ClearAbilitiesOfSlot(Slot);
			//ClearSlot(AbilitySpec);
			//AbilitySpec->DynamicAbilityTags.AddTag(Slot);
			//if (Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
			//{
			//	AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.Abilities_Status_Unlocked);
			//	AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.Abilities_Status_Equipped);
			//}


		}

		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
	}
}

void UAueaAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

bool UAueaAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (const auto AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (auto AueaAbility = Cast<UAueaGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription = AueaAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = AueaAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}

	const auto AbilityInfo = UAueaAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FAueaGameplayTags::Get().Abilities_NONE))
		OutDescription = FString();
	else
		OutDescription = UAueaGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	OutNextLevelDescription = FString();
	
	return false;
}

void UAueaAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* Spec)
{
	const auto Slot = GetInputTagFromSpec(*Spec);
	Spec->DynamicAbilityTags.RemoveTag(Slot);
}

void UAueaAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (auto& Spec : GetActivatableAbilities())
		if (AbilityHasSlot(&Spec, Slot))
			ClearSlot(&Spec);
}

bool UAueaAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot)
{
	for (auto Tag : Spec->DynamicAbilityTags)
		if (Tag.MatchesTagExact(Slot))
			return true;

	return false;
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

void UAueaAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer; 
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

void UAueaAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}