#include "Character/AueaCharacter.h"
#include "Player/AueaPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"

AAueaCharacter::AAueaCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AAueaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo(); // Init ability actor info for the Server
}

void AAueaCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo(); // Init ability actor info for the Client
}

void AAueaCharacter::InitAbilityActorInfo()
{
	AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	AueaPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AueaPlayerState, this);
	AbilitySystemComponent = AueaPlayerState->GetAbilitySystemComponent();
	AttributeSet = AueaPlayerState->GetAttributeSet();
}

