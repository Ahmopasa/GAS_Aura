// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AueaEnemy.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/AueaAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AueaUserWidget.h"
#include "AueaGameplayTags.h"
#include "AI/AueaAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAueaEnemy::AAueaEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAueaAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AttributeSet = CreateDefaultSubobject<UAueaAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	BaseWalkSpeed = 250.f;

	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();
}

void AAueaEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;
	AueaAIController = Cast<AAueaAIController>(NewController);

	AueaAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AueaAIController->RunBehaviorTree(BehaviorTree);
	AueaAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AueaAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);

}

void AAueaEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);

}

void AAueaEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);

	Weapon->SetRenderCustomDepth(false);
}

void AAueaEnemy::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	// DO NOT CHANGE OutDestination
}

int32 AAueaEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

void AAueaEnemy::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);

	if (AueaAIController) AueaAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);

	Super::Die(DeathImpulse);
}

void AAueaEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAueaEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAueaEnemy::HitReactChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (AueaAIController && AueaAIController->GetBlackboardComponent())
	{
		AueaAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}

void AAueaEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	InitAbilityActorInfo();

	if (HasAuthority()) UAueaAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);

	if (auto* AueaUserWidget = Cast<UAueaUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AueaUserWidget->SetWidgetController(this);
	}

	if (const UAueaAttributeSet* AueaAS = Cast<UAueaAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); }
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AueaAS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
		);

		AbilitySystemComponent->RegisterGameplayTagEvent(FAueaGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AAueaEnemy::HitReactChanged
		);

		OnHealthChanged.Broadcast(AueaAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AueaAS->GetMaxHealth());
	}
}

void AAueaEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAueaGameplayTags::Get().Debuff_Stun,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&AAueaEnemy::StunTagChanged
	);
	
	if (HasAuthority()) InitializeDefaultAttributes();

	OnASCRegistered.Broadcast(AbilitySystemComponent);
}

void AAueaEnemy::InitializeDefaultAttributes() const
{
	UAueaAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAueaEnemy::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);

	if (AueaAIController && AueaAIController->GetBlackboardComponent())
	{
		AueaAIController->GetBlackboardComponent()->SetValueAsBool(FName("Stunned"), bIsStunned);
	}
}
