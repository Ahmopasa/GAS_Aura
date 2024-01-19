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
class USplineComponent;
class UDamageTextComponent;
class UNiagaraSystem;
class AMagicCircle;

UCLASS()
class AURA_API AAueaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAueaPlayerController();
	
	virtual void PlayerTick(float DeltaTime) override;
	
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AueaContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	FORCEINLINE void ShiftPressed() { bShiftKeyDown = true; }
	FORCEINLINE void ShiftReleased() { bShiftKeyDown = false; }
	bool bShiftKeyDown = false;

	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagHold(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAueaInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAueaAbilitySystemComponent> AueaAbilitySystemComponent;
	UAueaAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem; 

	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;

	void UpdateMagicCircleLocation();
};
