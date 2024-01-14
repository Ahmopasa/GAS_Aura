#include "AbilitySystem/AueaAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AueaAbilityTypes.h"
#include "AueaGameplayTags.h"
#include "Game/AueaGameModeBase.h"
#include <Interaction/CombatInterface.h>
#include "Kismet/GameplayStatics.h"
#include "Player/AueaPlayerState.h"
#include "UI/HUD/AueaHUD.h"
#include "UI/WidgetController/AueaWidgetController.h"


bool UAueaAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AAueaHUD*& OutAueaHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAueaHUD = Cast<AAueaHUD>(PC->GetHUD());
		if (OutAueaHUD)
		{
			AAueaPlayerState* PS = PC->GetPlayerState<AAueaPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}

	return false;
}

UOverlayWidgetController* UAueaAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams; 
	AAueaHUD* AueaHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AueaHUD)) 
		return AueaHUD->GetOverlayWidgetController(WCParams);

	return nullptr; 
}

UAttributeMenuWidgetController* UAueaAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAueaHUD* AueaHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AueaHUD)) 
		return AueaHUD->GetAttributeMenuWidgetController(WCParams);

	return nullptr;
}

USpellMenuWidgetController* UAueaAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAueaHUD* AueaHUD = nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AueaHUD)) 
		return AueaHUD->GetSpellMenuWidgetController(WCParams);

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

void UAueaAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	if (UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
			ASC->GiveAbility(AbilitySpec);
		}

		const auto& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
		for (auto AbilityClass : DefaultInfo.StartupAbilities)
		{
			if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
			{
				FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
				ASC->GiveAbility(AbilitySpec);  
			}
		}
	}
}

UCharacterClassInfo* UAueaAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAueaGameModeBase* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AueaGameMode == nullptr) return nullptr;

	return AueaGameMode->CharacterClassInfo;
}

UAbilityInfo* UAueaAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAueaGameModeBase* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AueaGameMode == nullptr) return nullptr;

	return AueaGameMode->AbilityInfo;
}

bool UAueaAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->IsBlockedHit();

	return false;
}

bool UAueaAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->IsCriticalHit();

	return false;
}

bool UAueaAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->IsSuccessfulDebuff();

	return false;
}

float UAueaAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->GetDebuffDamage();

	return 0.f;
}

float UAueaAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->GetDebuffFrequency();

	return 0.f;
}

float UAueaAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->GetDebuffDuration();

	return 0.f;
}

FGameplayTag UAueaAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()); AueaEffectContext->GetDamageType().IsValid())
		return *AueaEffectContext->GetDamageType();

	return FGameplayTag();
}

FVector UAueaAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AuraEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AuraEffectContext->GetDeathImpulse();

	return FVector::ZeroVector;
}

FVector UAueaAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const auto* AueaEffectContext = static_cast<const FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		return AueaEffectContext->GetKnockbackForce();

	return FVector::ZeroVector;
}

void UAueaAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
}

void UAueaAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
}

void UAueaAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bInSuccessfulDebuff)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetIsSuccessfulDebuff(bInSuccessfulDebuff);
}

void UAueaAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDamage)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetDebuffDamage(InDebuffDamage);
}

void UAueaAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffFrequency)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetDebuffFrequency(InDebuffFrequency);
}

void UAueaAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDebuffDuration)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetDebuffDuration(InDebuffDuration);
}

void UAueaAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const auto DamageType = MakeShared<FGameplayTag>(InDamageType);
		AuraEffectContext->SetDamageType(DamageType);
	}
}

void UAueaAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InImpulse)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InImpulse);
	}
}

void UAueaAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InForce)
{
	if (auto* AuraEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
		AuraEffectContext->SetKnockbackForce(InForce);
}

void UAueaAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;

	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (const auto* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(
			Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams
		);

		for (auto& Overlap : Overlaps)
		{
			auto TheActor = Overlap.GetActor();
			if (TheActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(TheActor))
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(TheActor));
		}
	}
}

void UAueaAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors;
	int32 NumTargetsFound = 0;

	while (NumTargetsFound < MaxTargets)
	{
		if (ActorsToCheck.Num() == 0)
			break;

		auto ClosestDistance = TNumericLimits<double>::Max();
		AActor* ClosestActor;
		for (auto* PotentialTarget : ActorsToCheck)
		{
			const auto Distance = (PotentialTarget->GetActorLocation() - Origin).Length();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}

		ActorsToCheck.Remove(ClosestActor);
		OutClosestTargets.AddUnique(ClosestActor);
		++NumTargetsFound;
	}
}

bool UAueaAbilitySystemLibrary::IsNotFriend(AActor* First, AActor* Second)
{
	const bool bIsFirstAPlayer = First->ActorHasTag(FName("Player"));
	const bool bIsSecondAPlayer = Second->ActorHasTag(FName("Player"));
	const bool bIsFirstAnEnemy = First->ActorHasTag(FName("Enemy"));
	const bool bIsSecondAnEnemy = Second->ActorHasTag(FName("Enemy"));

	const bool bBothArePlayers = bIsFirstAPlayer && bIsSecondAPlayer;
	const bool bBothAreEnemies = bIsFirstAnEnemy && bIsSecondAnEnemy;

	const bool bFriends = bBothArePlayers || bBothAreEnemies;

	return !bFriends;
}

FGameplayEffectContextHandle UAueaAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{	
	const auto& GameplayTags = FAueaGameplayTags::Get();
	const auto* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	auto EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	const auto SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Properties_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Properties_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Properties_Frequency, DamageEffectParams.DebuffFrequency);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Properties_Duration, DamageEffectParams.DebuffDuration);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;
}

TArray<FRotator> UAueaAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators; 
	const auto LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const auto DeltaSpread = Spread / (NumRotators - 1);
		for (auto i = 0; i < NumRotators; ++i)
		{
			const auto Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}

	return Rotators; 
}

TArray<FVector> UAueaAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	const auto LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const auto DeltaSpread = Spread / (NumVectors - 1);
		for (auto i = 0; i < NumVectors; ++i)
		{
			const auto Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}

	return Vectors;
}

float UAueaAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	auto* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const auto& Info = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	return Info.XPReward.GetValueAtLevel(CharacterLevel);
}

//void UAueaAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
//{
//	if (auto* AueaEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
//		AueaEffectContext->SetIsBlockedHit(bInIsBlockedHit);
//}
//
//void UAueaAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
//{
//	if (auto* AueaEffectContext = static_cast<FAueaGameplayEffectContext*>(EffectContextHandle.Get()))
//		AueaEffectContext->SetIsCriticalHit(bInIsCriticalHit);
//}
