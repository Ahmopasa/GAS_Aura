#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAueaAttributeSet* AueaAttributeSet = CastChecked<UAueaAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(AueaAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AueaAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AueaAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AueaAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAueaAttributeSet* AueaAttributeSet = CastChecked<UAueaAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); }
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);

	if (auto* AueaASC = Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (AueaASC->bStartupAbilitiesGiven) OnInitializeStartupAbilities(AueaASC);
		else AueaASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);

		AueaASC->EffectAssetTags.AddLambda(
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAueaAbilitySystemComponent* AueaAbilitySystemComponent)
{
	if (!AueaAbilitySystemComponent->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this, AueaAbilitySystemComponent] (const FGameplayAbilitySpec& AbilitySpec) {
			auto Info = AbilityInfo->FindAbilityInfoForTag(AueaAbilitySystemComponent->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = AueaAbilitySystemComponent->GetInputTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	AueaAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}
