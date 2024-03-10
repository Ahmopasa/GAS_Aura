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

	LoadScreenWidget->BlueprintInitializeWidget();
}
