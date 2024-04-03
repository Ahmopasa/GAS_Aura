// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AueaEnemy.h"
#include "Actor/AueaEnemySpawnPoint.h"

void AAueaEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	auto* Enemy = GetWorld()->SpawnActorDeferred<AAueaEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
