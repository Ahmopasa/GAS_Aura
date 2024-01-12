#include "Player/AueaPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AueaInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Components/SplineComponent.h"
#include "AueaGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"
#include <NiagaraFunctionLibrary.h>

AAueaPlayerController::AAueaPlayerController()
{
	/*
	*	Replication is responding to data; updating on the server
	*	and sending that down to clients. 
	*/
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAueaPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();
}

void AAueaPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			15.0f,
			FColor::Yellow,
			FString::Printf(TEXT("ShowDamageNumber_Implementation_Damage: %f"), DamageAmount)
		);

		auto* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
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
	AueaInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAueaPlayerController::ShiftPressed);
	AueaInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAueaPlayerController::ShiftReleased);
	AueaInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHold);
}

void AAueaPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputPressed))
		return;

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
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_CursorTrace))
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->UnHighlightActor();
		
		ThisActor = LastActor = nullptr;
		
		return;
	}

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return; 

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAueaPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputPressed))
		return;

	if (InputTag.MatchesTagExact(FAueaGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}

	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AAueaPlayerController::AbilityInputTagHold(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputHeld))
		return;

	if (!InputTag.MatchesTagExact(FAueaGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);

		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

void AAueaPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputReleased))
		return;

	if (!InputTag.MatchesTagExact(FAueaGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

		return;
	}
	
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLocation : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}

			if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputPressed))
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					this, ClickNiagaraSystem, CachedDestination
				);

		}
		FollowTime = 0.f;
		bTargeting = false;
	}
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

void AAueaPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

