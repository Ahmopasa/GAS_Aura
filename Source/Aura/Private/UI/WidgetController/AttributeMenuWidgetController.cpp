#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

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
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAueaAttributeSet* AS = CastChecked<UAueaAttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair : AS->TagsToAttributes)
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FAueaAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
