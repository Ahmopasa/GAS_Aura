#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AueaGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{

}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAueaAttributeSet* AS = CastChecked<UAueaAttributeSet>(AttributeSet);
	check(AttributeInfo);
	FAueaAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAueaGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}
