// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AueaCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class AURA_API AAueaCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAueaCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; 
	UAttributeSet* GetAttributeSet() const; 

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<USkeletalMeshComponent> Weapon; 

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes; 

	void InitializePrimaryAttributes() const;
};
