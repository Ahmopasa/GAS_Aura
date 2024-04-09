// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AueaCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AueaEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAueaAIController;

UCLASS()
class AURA_API AAueaEnemy : public AAueaCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAueaEnemy();

	virtual void PossessedBy(AController* NewController) override;

	//~begin Highlight Interface
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	//~end Highlight Interface

	//~begin Combat Interface
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	//~end Combat Interface

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	FORCEINLINE void SetLevel(int32 InLevel) { Level = InLevel; }

protected:
	virtual void BeginPlay()override;

	virtual void InitAbilityActorInfo() override;

	virtual void InitializeDefaultAttributes() const override;

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAueaAIController> AueaAIController;
};
