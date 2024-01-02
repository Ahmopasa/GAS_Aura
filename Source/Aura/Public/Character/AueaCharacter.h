// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AueaCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AueaCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAueaCharacter : public AAueaCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAueaCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState()override;

	//~begin Player Interface
	virtual void AddToXP_Implementation(int32 InXP) override;
	//~end Player Interface

	//~begin Combat Interface
	virtual int32 GetPlayerLevel() override;
	//~end Combat Interface

private:
	virtual void InitAbilityActorInfo() override;
};
