// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	
	/*
	*	Load Screen
	*/

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LoadScreenWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ULoadScreenWidget> LoadScreenWidget;

	/*
	*	Load Screen | View Model
	*/

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMVVM_LoadScreen> LoadScreenViewModelClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_LoadScreen> LoadScreenViewModel;

protected:
	virtual void BeginPlay() override;
};
