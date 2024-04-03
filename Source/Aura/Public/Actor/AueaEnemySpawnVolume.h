// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AueaEnemySpawnVolume.generated.h"

UCLASS()
class AURA_API AAueaEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:	

	AAueaEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* Save Interface */

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	TArray<class AAueaEnemySpawnPoint*> SpawnPoints;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> Box;
};
