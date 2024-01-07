#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AueaPlayerState.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AueaGameplayTags.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	
	OnHealthChanged.Broadcast(GetAueaAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAueaAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAueaAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAueaAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAueaPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAueaPS()->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel) 
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAueaAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAueaAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAueaAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAueaAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);

	if (GetAueaASC())
	{
		GetAueaASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

		if (GetAueaASC()->bStartupAbilitiesGiven) BroadcastAbilityInfo();
		else GetAueaASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);

		GetAueaASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags) 
			{
				for (const auto& Tag : AssetTags)
				{
					// Tag is Message.HealthPotion.
					// Then,
					// "Message.HealthPotion".MatchesTag("Message") will return True. 
					// "Message".MatchesTag("Message.HealthPotion") will return False.
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAueaPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo. Please, fill out AueaPlayerState blueprint."));
	
	const auto Level = LevelUpInfo->FindLevelForXP(NewXP);
	const auto MaxLevel = LevelUpInfo->LevelUpInformation.Num();
	if (Level <= MaxLevel && Level > 0)
	{
		const auto LevelUpRequiretment = LevelUpInfo->LevelUpInformation[Level].LevelUpRequiretment;
		const auto PreviousLevelUpRequiretment = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequiretment;
		const auto DeltaLevelRequiretment = LevelUpRequiretment - PreviousLevelUpRequiretment;
		const auto XPForThisLevel = NewXP - PreviousLevelUpRequiretment;
		const auto XPBarPercent = static_cast<float>(XPForThisLevel) / DeltaLevelRequiretment;
		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}

}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot) const
{
	const auto& GameplayTags = FAueaGameplayTags::Get();
	FAueaAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_NONE;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	auto Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);
}
