#include "Character/AueaCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"
#include "AueaGameplayTags.h"
#include <Kismet/GameplayStatics.h>
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

AAueaCharacterBase::AAueaCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAueaGameplayTags::Get().Debuff_Burn;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap); 
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAueaCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAueaCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

UAnimMontage* AAueaCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAueaCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	MulticastHandleDeath(DeathImpulse);
}


void AAueaCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UE_LOG(LogTemp, Warning, TEXT("MulticastHandleDeath_Implementation >> Impulse Magnitude: [%s]"), *DeathImpulse.ToString());
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse, NAME_None, true);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();

	bDead = true;

	OnDeath.Broadcast(this);
}

void AAueaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAueaCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const auto& GameplayTags = FAueaGameplayTags::Get();

	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}

	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}

	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}

	return FVector();
}

bool AAueaCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAueaCharacterBase::GetAvatar_Implementation() 
{
	return this;
}

TArray<FTaggedMontage> AAueaCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAueaCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect; 
}

FTaggedMontage AAueaCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (auto TaggedMontage : AttackMontages)
		if (TaggedMontage.MontageTag == MontageTag)
			return TaggedMontage;

	return FTaggedMontage();
}

int32 AAueaCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount; 
}

void AAueaCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount; 
}

ECharacterClass AAueaCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered AAueaCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeath* AAueaCharacterBase::GetOnDeathDelegate()
{
	return &OnDeath;
}

void AAueaCharacterBase::InitAbilityActorInfo()
{

}

void AAueaCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffect);

	auto ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const auto SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAueaCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAueaCharacterBase::AddCharacterAbilities()
{
	UAueaAbilitySystemComponent* AueaASC = CastChecked<UAueaAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;

	AueaASC->AddCharacterAbilities(StartupAbilities);

	AueaASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAueaCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);

		StartDissolveTimeLine(DynamicMatInst);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);

		StartWeaponDissolveTimeLine(DynamicMatInst);
	}

}


