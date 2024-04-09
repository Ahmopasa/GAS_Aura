#include "Actor/AueaEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <Kismet/KismetMathLibrary.h>

AAueaEffectActor::AAueaEffectActor()
{
 	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAueaEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (const auto SinePeriod = 2 * PI / SinePeriodConstant; RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}

	ItemMovement(DeltaTime);
}

void AAueaEffectActor::BeginPlay()
{
	Super::BeginPlay();

	CalculatedLocation = InitialLocation = GetActorLocation();

	CalculatedRotation = GetActorRotation();
}

void AAueaEffectActor::StartRotation()
{
	bRotates = true;
	
	CalculatedRotation = GetActorRotation();
}

void AAueaEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;

	CalculatedLocation = InitialLocation = GetActorLocation();
}

void AAueaEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;
	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	if (!bIsInfinite)
	{
		Destroy();
	}
}

void AAueaEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAueaEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles) 
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);

		}
	}
}

void AAueaEffectActor::ItemMovement(float DeltaTime)
{
	if (bRotates)
	{
		const auto DeltaRotation = FRotator(0.f, DeltaTime * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}

	if (bSinusoidalMovement)
	{
		const auto Sine = SineAmplitude * FMath::Sin(SinePeriodConstant * RunningTime);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
}

