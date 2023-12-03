// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AueaEnemy.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/AueaAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AueaUserWidget.h"

AAueaEnemy::AAueaEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAueaAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAueaAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AAueaEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAueaEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);

	Weapon->SetRenderCustomDepth(false);
}

int32 AAueaEnemy::GetPlayerLevel()
{
	return Level;
}

void AAueaEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();

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

		OnHealthChanged.Broadcast(AueaAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AueaAS->GetMaxHealth());
	}
}

void AAueaEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	InitializeDefaultAttributes();
}

void AAueaEnemy::InitializeDefaultAttributes() const
{
	UAueaAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}
