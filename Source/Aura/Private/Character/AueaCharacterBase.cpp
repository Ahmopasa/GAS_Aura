// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AueaCharacterBase.h"


AAueaCharacterBase::AAueaCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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


void AAueaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAueaCharacterBase::InitAbilityActorInfo()
{

}


