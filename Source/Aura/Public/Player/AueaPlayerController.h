// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AueaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAueaInputConfig;
class UAueaAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAueaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAueaPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AueaContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;


	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHold(FGameplayTag InputTag);
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAueaInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAueaAbilitySystemComponent> AueaAbilitySystemComponent;
	UAueaAbilitySystemComponent* GetASC();
};
