#include "Player/AueaPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AueaGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AueaInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"
#include <Interaction/HighlightInterface.h>


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

	UpdateMagicCircleLocation();
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

void AAueaPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
}

void AAueaPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
		MagicCircle->Destroy();
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
		UnhighlightActor(LastActor);
		UnhighlightActor(ThisActor);

		ThisActor = LastActor = nullptr;
		
		return;
	}

	const auto TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return; 

	LastActor = ThisActor;
	if (IsValid(CursorHit.GetActor()) && CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}
	else
	{
		ThisActor = nullptr;
	}
	

	if (LastActor != ThisActor)
	{
		UnhighlightActor(LastActor);
		HighlightActor(ThisActor);
	}
}

void AAueaPlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(InActor);
	}
}

void AAueaPlayerController::UnhighlightActor(AActor* InActor)
{
	if (IsValid(InActor) && InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(InActor);
	}
}

void AAueaPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputPressed))
		return;

	if (InputTag.MatchesTagExact(FAueaGameplayTags::Get().InputTag_LMB))
	{
		if (IsValid(ThisActor))
		{
			TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;
			bAutoRunning = false;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NoTargeting;
		}
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

	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
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
	
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (IsValid(ThisActor) && ThisActor->Implements<UHighlightInterface>())
			{
				IHighlightInterface::Execute_SetMoveToLocation(ThisActor, CachedDestination);
			}
			else
			{
				if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAueaGameplayTags::Get().InputTag_Block_InputPressed))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						this, ClickNiagaraSystem, CachedDestination
					);
				}
			}

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

			

		}
		FollowTime = 0.f;
		TargetingStatus = ETargetingStatus::NoTargeting;
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

void AAueaPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
}

