// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AueaEnemySpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAueaEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	TSubclassOf<class AAueaEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	int32 EnemyLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

};
