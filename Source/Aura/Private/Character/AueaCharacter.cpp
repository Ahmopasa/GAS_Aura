#include "Character/AueaCharacter.h"
#include "Player/AueaPlayerState.h"
#include "Player/AueaPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "UI/HUD/AueaHUD.h"
#include <NiagaraComponent.h>
#include "Camera/CameraComponent.h"
#include <AueaGameplayTags.h>
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

AAueaCharacter::AAueaCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;


	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void AAueaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo(); // Init ability actor info for the Server
	AddCharacterAbilities();
}

void AAueaCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo(); // Init ability actor info for the Client
}

int32 AAueaCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

void AAueaCharacter::AddToXP_Implementation(int32 InXP)
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->AddToXP(InXP);
}

int32 AAueaCharacter::GetXP_Implementation() const
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->GetXP();
}

void AAueaCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->AddToLevel(InPlayerLevel);

	if (auto ASC = Cast<UAueaAbilitySystemComponent>(GetAbilitySystemComponent()))
		ASC->UpdateAbilityStatus(AueaPlayerState->GetPlayerLevel());

}

void AAueaCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AAueaCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;

	//AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	//check(AueaPlayerState);
	//
	//auto PlayerLastLevel = AueaPlayerState->GetPlayerLevel();
	//auto AttributePoints = 0;
	//while (PlayerLastLevel-- < Level)
	//	AttributePoints += AueaPlayerState->LevelUpInfo->LevelUpInformation[PlayerLastLevel].AttributePointAward;
	//	
	//return AttributePoints;
}

void AAueaCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->AddToAttributePoints(InAttributePoints);
}

int32 AAueaCharacter::GetAttributePoints_Implementation() const
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->GetAttributePoints();
}

int32 AAueaCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void AAueaCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAueaCharacter::GetSpellPoints_Implementation() const
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->GetSpellPoints();
}

int32 AAueaCharacter::GetPlayerLevel_Implementation()
{
	const AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->GetPlayerLevel();
}

void AAueaCharacter::OnRep_Stunned()
{
	if (auto* AueaASC = Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent))
	{
		const auto& GameplayTags = FAueaGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.InputTag_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.InputTag_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.InputTag_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.InputTag_Block_InputReleased);
		if (bIsStunned)
		{
			AueaASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			AueaASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAueaCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AAueaCharacter::InitAbilityActorInfo()
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AueaPlayerState, this);
	Cast<UAueaAbilitySystemComponent>(AueaPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AueaPlayerState->GetAbilitySystemComponent();
	AttributeSet = AueaPlayerState->GetAttributeSet();
	
	OnASCRegistered.Broadcast(AbilitySystemComponent); 

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAueaGameplayTags::Get().Debuff_Stun, 
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&AAueaCharacter::StunTagChanged
	);

	if (AAueaPlayerController* AueaPlayerController = Cast<AAueaPlayerController>(GetController())) {
		if (AAueaHUD* AueaHUD = Cast<AAueaHUD>(AueaPlayerController->GetHUD())) {
			AueaHUD->InitOverlay(AueaPlayerController, AueaPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	
	InitializeDefaultAttributes();
}

void AAueaCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const auto CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const auto NiagaSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const auto ToCameraRotation = (CameraLocation - NiagaSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}

}

