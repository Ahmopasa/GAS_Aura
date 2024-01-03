#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AueaPlayerState.h"
#include <AbilitySystem/AueaAbilitySystemComponent.h>

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAueaAttributeSet* AS = CastChecked<UAueaAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair : AS->TagsToAttributes)
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair, AS](const FOnAttributeChangeData& Data) 
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);

	auto* AueaPlayerState = CastChecked<AAueaPlayerState>(PlayerState);
	AueaPlayerState->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 Points) 
		{
			AttributePointsChangedDelegate.Broadcast(Points);
		}
	);
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAueaAttributeSet* AS = CastChecked<UAueaAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair : AS->TagsToAttributes)
		BroadcastAttributeInfo(Pair.Key, Pair.Value());

	auto* AueaPlayerState = CastChecked<AAueaPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(AueaPlayerState->GetAttributePoints());
}

void UAttributeMenuWidgetController::UpgradeAttributes(const FGameplayTag& AttributeTag)
{
	auto* AueaASC = CastChecked<UAueaAbilitySystemComponent>(AbilitySystemComponent);
	AueaASC->UpgradeAttributes(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAueaAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
