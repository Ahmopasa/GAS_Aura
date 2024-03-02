#include "Actor/AueaProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "AbilitySystemComponent.h"
#include <AbilitySystem/AueaAbilitySystemLibrary.h>

AAueaProjectile::AAueaProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	// Sphere Component
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Projectile Movement Component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAueaProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAueaProjectile::OnSphereOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAueaProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	if (!bHit && !HasAuthority()) OnHit();

	Super::Destroyed();
}

void AAueaProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	
	bHit = true;
}

void AAueaProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;
	if (!bHit) OnHit();
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// Handling Death Impulse: 
			const auto DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			UE_LOG(LogTemp, Warning, TEXT("OnSphereOverlap >> DamageEffectParams.DeathImpulseMagnitude's Magnitude: [%f]"), DamageEffectParams.DeathImpulseMagnitude);
			DamageEffectParams.DeathImpulse = DeathImpulse;
			UE_LOG(LogTemp, Warning, TEXT("OnSphereOverlap >> DamageEffectParams.DeathImpulse's Magnitude: [%s]"), *DamageEffectParams.DeathImpulse.ToString());

			// Handling Knockbacking: 
			if (FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance)
			{
				auto Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				const auto KnockbackDirection = Rotation.Vector();
				const auto KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
				DamageEffectParams.KnockbackForce = KnockbackForce;
			}

			// Applying the effects: 
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAueaAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}

		Destroy();
	}
	else
	{
		bHit = true;
	}
}

bool AAueaProjectile::IsValidOverlap(AActor* OtherActor)
{
	if (DamageEffectParams.SourceAbilitySystemComponent == nullptr) return false;
	auto* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor) return false;
	if (!UAueaAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return false;

	return true;
}

