#include "Character/AueaCharacter.h"
#include "Player/AueaPlayerState.h"
#include "Player/AueaPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "UI/HUD/AueaHUD.h"

AAueaCharacter::AAueaCharacter()
{
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

void AAueaCharacter::AddToXP_Implementation(int32 InXP)
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->AddToXP(InXP);
}

int32 AAueaCharacter::GetPlayerLevel()
{
	const AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->GetPlayerLevel();
}

void AAueaCharacter::InitAbilityActorInfo()
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AueaPlayerState, this);
	Cast<UAueaAbilitySystemComponent>(AueaPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AueaPlayerState->GetAbilitySystemComponent();
	AttributeSet = AueaPlayerState->GetAttributeSet();

	if (AAueaPlayerController* AueaPlayerController = Cast<AAueaPlayerController>(GetController())) {
		if (AAueaHUD* AueaHUD = Cast<AAueaHUD>(AueaPlayerController->GetHUD())) {
			AueaHUD->InitOverlay(AueaPlayerController, AueaPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	
	InitializeDefaultAttributes();
}

