#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"

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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetHealthAttribute()).AddUObject(
		this, 
		&UOverlayWidgetController::HealthChanged
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetMaxHealthAttribute()).AddUObject(
		this,
		&UOverlayWidgetController::MaxHealthChanged
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetManaAttribute()).AddUObject(
		this,
		&UOverlayWidgetController::ManaChanged
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAttributeSet->GetMaxManaAttribute()).AddUObject(
		this,
		&UOverlayWidgetController::MaxManaChanged
	);

	Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags) 
		{
			for (const auto& Tag : AssetTags)
			{
				const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Msg);
			}
		}
	);
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
