// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AueaCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AueaEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAueaEnemy : public AAueaCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAueaEnemy();

	//~begin Enemy Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//~end Enemy Interface

protected:
	virtual void BeginPlay()override;
};
