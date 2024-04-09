// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AueaEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy 
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

UCLASS()
class AURA_API AAueaEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAueaEffectActor();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movements")
	bool bRotates = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movements")
	float RotationRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movements")
	bool bSinusoidalMovement = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movements")
	float SineAmplitude = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movements")
	float SinePeriodConstant = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FRotator CalculatedRotation;

	UFUNCTION(BlueprintCallable)
	void StartRotation();

	UPROPERTY(BlueprintReadWrite)
	FVector CalculatedLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movements")
	FVector InitialLocation;

	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bApplyEffectsToEnemies = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	float ActorLevel = 1.f;

private:
	float RunningTime = 0.f;

	void ItemMovement(float DeltaTime);
};
