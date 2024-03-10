// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AueaGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAueaGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, SlotIndex);
	}

	auto* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavedGameClass);
	auto* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->PlayerName;
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}
