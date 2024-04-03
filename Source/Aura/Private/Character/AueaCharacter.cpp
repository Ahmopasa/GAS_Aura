#include "AbilitySystemComponent.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include <AbilitySystem/AueaAttributeSet.h>
#include <AbilitySystem/AueaAbilitySystemLibrary.h>
#include <AueaGameplayTags.h>
#include "Camera/CameraComponent.h"
#include "Character/AueaCharacter.h"
#include <Game/AueaGameModeBase.h>
#include <Game/AueaGameInstance.h>
#include <Game/LoadScreenSaveGame.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include <Kismet/GameplayStatics.h>
#include <NiagaraComponent.h>
#include "Player/AueaPlayerState.h"
#include "Player/AueaPlayerController.h"
#include "UI/HUD/AueaHUD.h"

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

	LoadProgress();

	if (auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AueaGameMode->LoadWorldState(GetWorld());
	}
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

void AAueaCharacter::ShowMagicCircle_Implementation(UMaterialInstance* DecalMaterial)
{
	if (auto* AueaPlayerController = Cast<AAueaPlayerController>(GetController()))
	{
		AueaPlayerController->ShowMagicCircle(DecalMaterial);
		AueaPlayerController->bShowMouseCursor = false;
	}
}

void AAueaCharacter::HideMagicCircle_Implementation()
{
	if (auto* AueaPlayerController = Cast<AAueaPlayerController>(GetController()))
	{
		AueaPlayerController->HideMagicCircle();
		AueaPlayerController->bShowMouseCursor = true;
	}
}

void AAueaCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AueaGameMode)
	{
		auto SaveData = AueaGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		SaveData->bFirstTimeLoading = false;

		/*
		*	Saving Player Stats
		*/
		SaveData->PlayerStartTag = CheckpointTag;
		if (auto* AueaPlayerState = Cast<AAueaPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = AueaPlayerState->GetPlayerLevel();
			SaveData->XP = AueaPlayerState->GetXP();
			SaveData->SpellPoints = AueaPlayerState->GetSpellPoints();
			SaveData->AttributePoints = AueaPlayerState->GetAttributePoints();
		}

		/*
		*	Saving Primary Attributes
		*/
		SaveData->Strength = UAueaAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UAueaAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = UAueaAttributeSet::GetResilinceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = UAueaAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

		/*
		*	Saving Abilities
		*/
		if (!HasAuthority()) return;
		auto* AueaASC = Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda(
			[this, AueaASC, SaveData](const FGameplayAbilitySpec& AbilitySpec) {
				const auto AbilityTag = AueaASC->GetAbilityTagFromSpec(AbilitySpec);
				auto* AbilityInfo = UAueaAbilitySystemLibrary::GetAbilityInfo(this);
				auto Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				
				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbility = Info.Ability;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilitySlot = AueaASC->GetSlotFromAbilityTag(AbilityTag);
				SavedAbility.AbilityStatus = AueaASC->GetStatusFromAbilityTag(AbilityTag);
				SavedAbility.AbilityTag = AbilityTag;
				SavedAbility.AbilityType = Info.AbilityType;

				SaveData->SavedAbilities.AddUnique(SavedAbility);
			}
		);
		AueaASC->ForEachAbility(SaveAbilityDelegate);

		AueaGameMode->SaveInGameProgressData(SaveData);
	}
}

int32 AAueaCharacter::GetPlayerLevel_Implementation()
{
	const AAueaPlayerState* AueaPlayerState = GetPlayerState<AAueaPlayerState>();
	check(AueaPlayerState);
	return AueaPlayerState->GetPlayerLevel();
}

void AAueaCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);

	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]() {
		auto* AueaGM = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (AueaGM)
		{
			AueaGM->PlayerDied(this);
		}
	});

	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);

	TopDownCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
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

void AAueaCharacter::LoadProgress()
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (AueaGameMode)
	{
		auto SaveData = AueaGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		if (SaveData->bFirstTimeLoading)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			if (auto* AueaASC = Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent))
			{
				AueaASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}

			if (auto* AueaPlayerState = Cast<AAueaPlayerState>(GetPlayerState()))
			{
				AueaPlayerState->SetLevel(SaveData->PlayerLevel);
				AueaPlayerState->SetXP(SaveData->XP);
				AueaPlayerState->SetSpellPoints(SaveData->SpellPoints);
				AueaPlayerState->SetAttributePoints(SaveData->AttributePoints);
			}

			UAueaAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(
				this, 
				AbilitySystemComponent, 
				SaveData
			);
		}
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

