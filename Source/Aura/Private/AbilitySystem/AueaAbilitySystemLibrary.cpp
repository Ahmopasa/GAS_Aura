#include "AbilitySystem/AueaAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AueaWidgetController.h"
#include "UI/HUD/AueaHUD.h"
#include "Player/AueaPlayerState.h"
#include "Game/AueaGameModeBase.h"
#include "AbilitySystemComponent.h"
#include <AueaAbilityTypes.h>

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

void UAueaAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AActor* AvatarActor = ASC->GetAvatarActor();
	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);

	if (UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
		const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());
	
		FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
		SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
	
		FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
		VitalAttributesContextHandle.AddSourceObject(AvatarActor);
		const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	}
}

void UAueaAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if (UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAueaAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAueaGameModeBase* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AueaGameMode == nullptr) return nullptr;
	return AueaGameMode->CharacterClassInfo;
}

bool UAueaAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AueaEffectContext->IsBlockedHit();
	}

	return false;
}

bool UAueaAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AueaEffectContext->IsCriticalHit();
	}

	return false;
}

void UAueaAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (auto* AueaEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AueaEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAueaAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (auto* AueaEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AueaEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}
