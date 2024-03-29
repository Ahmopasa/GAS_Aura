// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AueaGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "GameFramework/PlayerStart.h"
#include <Game/AueaGameInstance.h>

void AAueaGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->LoadSlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->LoadSlotName, SlotIndex);
	}

	auto* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavedGameClass);
	auto* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();	
	LoadScreenSaveGame->SaveSlotStatus = Taken;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->LoadSlotName, SlotIndex);
}

ULoadScreenSaveGame* AAueaGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSavedGameClass);
	}

	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

void AAueaGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

void AAueaGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(
		Slot, 
		Maps.FindChecked(Slot->GetMapName())
	);
}

AActor* AAueaGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	auto* AueaGameInstance = Cast<UAueaGameInstance>(GetGameInstance());

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0) {
		auto* SelectedActor = Actors[0];
		for (auto* Actor : Actors) {
			if (auto* PlayerStart = Cast<APlayerStart>(Actor); PlayerStart->PlayerStartTag == AueaGameInstance->PlayerStartTag) {
				SelectedActor = PlayerStart;
				break;
			}
		}

		return SelectedActor;
	}

	return nullptr;
}

void AAueaGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Maps.Add(DefaultMapName, DefaultMap);
}
