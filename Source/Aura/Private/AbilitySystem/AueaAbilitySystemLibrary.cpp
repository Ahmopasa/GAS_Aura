#include "AbilitySystem/AueaAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AueaWidgetController.h"
#include "UI/HUD/AueaHUD.h"
#include "Player/AueaPlayerState.h"

UOverlayWidgetController* UAueaAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAueaHUD* AueaHUD = Cast<AAueaHUD>(PC->GetHUD()))
		{
			AAueaPlayerState* PS = PC->GetPlayerState<AAueaPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AueaHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}

	return nullptr; 
}

UAttributeMenuWidgetController* UAueaAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAueaHUD* AueaHUD = Cast<AAueaHUD>(PC->GetHUD()))
		{
			AAueaPlayerState* PS = PC->GetPlayerState<AAueaPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AueaHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}

	return nullptr;
}
