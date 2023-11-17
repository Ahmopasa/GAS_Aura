// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AueaCharacterBase.h"
#include "AueaCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAueaCharacter : public AAueaCharacterBase
{
	GENERATED_BODY()

public:
	AAueaCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState()override;

private:
	virtual void InitAbilityActorInfo() override;
};
