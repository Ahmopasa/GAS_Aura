#include "Player/AueaPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "Input/AueaInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAueaPlayerController::AAueaPlayerController()
{
	/*
	*	Replication is responding to data; updating on the server
	*	and sending that down to clients. 
	*/
	bReplicates = true;
}

void AAueaPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAueaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*
	*	Halts the execution if the condition fails meaning that
	*	"AueaContext" must be VALID.
	*/
	check(AueaContext); 

	/*
	*	Set the input mode.
	*/
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
		Subsystem->AddMappingContext(AueaContext, 0);

	/*
	*	Set the cursor settings.
	*/
	bShowMouseCursor = true; 
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAueaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAueaInputComponent* AueaInputComponent = CastChecked<UAueaInputComponent>(InputComponent);
	AueaInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAueaPlayerController::Move);
	AueaInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHold);
}

void AAueaPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>()) {
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAueaPlayerController::CursorTrace()
{
	FHitResult CursorHit; 
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return; 

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	/*
	*	Lince tracing from cursor. There're several scenarios: 
	*	A. LastActor is null && ThisActor is null 
	*		- Do nothing
	* 
	*	B. LastActor is null && ThisActor is valid
	*		- Highlight ThisActor
	* 
	*	C. LastActor is valid && ThisActor is null
	*		- UnHighlight LastActor
	* 
	*	D. Both actors're valid but LastActor != ThisActor
	*		- UnHighlight LastActor
	*		- Highlight ThisActor
	* 
	*	E. Both actors're valid and LastActor == ThisActor
	*		- Do nothing
	*/
	if (LastActor == nullptr) {
		if (ThisActor) {
			// Case B 
			ThisActor->HighlightActor();

		}
		else {
			// Case A - do nothing 
		
		}
	}
	else { 
		// LastActor is valid
		
		if (ThisActor == nullptr) {
			// Case C 
			LastActor->UnHighlightActor();

		}
		else {
			// Both actors are valid. 
			if (LastActor != ThisActor) {
				// Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();

			}
			else {
				// Case E - do nothing 

			}
		}
	}
}

void AAueaPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAueaPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);

}

void AAueaPlayerController::AbilityInputTagHold(FGameplayTag InputTag)
{
	if (GetASC() == nullptr) return;
	GetASC()->AbilityInputTagHeld(InputTag);

}

UAueaAbilitySystemComponent* AAueaPlayerController::GetASC()
{
	if (AueaAbilitySystemComponent == nullptr)
	{
		AueaAbilitySystemComponent = Cast<UAueaAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())
		);
	}

	return AueaAbilitySystemComponent;
}

