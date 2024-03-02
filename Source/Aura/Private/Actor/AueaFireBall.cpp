// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AueaFireBall.h"
#include <AbilitySystemBlueprintLibrary.h>
#include <AbilitySystem/AueaAbilitySystemLibrary.h>

void AAueaFireBall::BeginPlay()
{
	Super::BeginPlay();

	StartOutgoingTimeline();
}

void AAueaFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// Handling Death Impulse: 
			const auto DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			// Applying the effects: 
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAueaAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
