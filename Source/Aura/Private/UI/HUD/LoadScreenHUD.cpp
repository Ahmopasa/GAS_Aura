// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LoadScreenHUD.h"
#include "Blueprint/UserWidget.h"
#include <UI/Widget/LoadScreenWidget.h>
#include "UI/ViewModel/MVVM_LoadScreen.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	/*
	*	Load Screen | View Model
	*/

	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(
		this,
		LoadScreenViewModelClass
	);

	LoadScreenViewModel->InitializeLoadSlots();

	/*
	*	Load Screen
	*/

	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(
		GetWorld(),
		LoadScreenWidgetClass
	);

	LoadScreenWidget->AddToViewport();

	// ALTERNATIVE => CONNECT "LetMouseFunctionOnly" NODE IN THE "WBP_LOADSCREEN". 
	if (auto* PlayerController = Cast<APlayerController>(GetOwningPlayerController()))
	{
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->bShowMouseCursor = true;
	}

	LoadScreenWidget->BlueprintInitializeWidget();

	/*
	*	Load Screen | View Model
	*/

	LoadScreenViewModel->LoadData();
}
