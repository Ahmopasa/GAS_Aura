// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AueaSummonAbility.h"
#include <Kismet/KismetSystemLibrary.h>

TArray<FVector> UAueaSummonAbility::GetSpawnLocations()
{
	const auto Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const auto LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const auto RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	const auto DeltaSpread = SpawnSpread / NumMinions;
	const auto Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FVector> SpawnLocations;
	for (auto i = 0; i < NumMinions; ++i)
	{
		const auto Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		auto ChoseSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			ChoseSpawnLocation + FVector(0.f, 0.f, 500.f),
			ChoseSpawnLocation - FVector(0.f, 0.f, 500.f),
			ECC_Visibility
		);
		if (Hit.bBlockingHit) ChoseSpawnLocation = Hit.ImpactPoint;
		
		SpawnLocations.Add(ChoseSpawnLocation);
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAueaSummonAbility::GetRandomMinionClass()
{
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
