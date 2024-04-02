// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AueaGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "GameFramework/PlayerStart.h"
#include <Game/AueaGameInstance.h>
#include "EngineUtils.h"
#include <Interaction/SaveInterface.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

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

ULoadScreenSaveGame* AAueaGameModeBase::RetrieveInGameSaveData()
{
	auto* AueaGameInstance = Cast<UAueaGameInstance>(GetGameInstance());
	const auto InGameLoadSlotName = AueaGameInstance->LoadSlotName;
	const auto InGameLoadSlotIndex = AueaGameInstance->LoadSlotIndex;
	return GetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAueaGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject)
{
	auto* AueaGameInstance = Cast<UAueaGameInstance>(GetGameInstance());
	const auto InGameLoadSlotName = AueaGameInstance->LoadSlotName;
	const auto InGameLoadSlotIndex = AueaGameInstance->LoadSlotIndex;
	AueaGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAueaGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	auto WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	auto* AueaGI = Cast<UAueaGameInstance>(GetGameInstance());
	check(AueaGI);
	if (auto* SaveGame = GetSaveSlotData(AueaGI->LoadSlotName, AueaGI->LoadSlotIndex))
	{
		if (DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
		}

		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);	 
		}

		auto SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty(); 
		for (FActorIterator It(World); It; ++It)
		{
			auto* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())
				continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			Actor->Serialize(Archive);

			SavedMap.SavedActors.AddUnique(SavedActor);
		}

		for (auto& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}

		UGameplayStatics::SaveGameToSlot(SaveGame, AueaGI->LoadSlotName, AueaGI->LoadSlotIndex);
	}
}

void AAueaGameModeBase::LoadWorldState(UWorld* World) const
{
	auto WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	auto* AueaGI = Cast<UAueaGameInstance>(GetGameInstance());
	check(AueaGI);

	if (UGameplayStatics::DoesSaveGameExist(AueaGI->LoadSlotName, AueaGI->LoadSlotIndex))
	{
		auto* SaveGame = Cast<ULoadScreenSaveGame>(
			UGameplayStatics::LoadGameFromSlot(
				AueaGI->LoadSlotName, 
				AueaGI->LoadSlotIndex)
		);
		if (SaveGame == nullptr) return;

		for (FActorIterator It(World); It; ++It)
		{
			auto* Actor = *It;

			if (!Actor->Implements<USaveInterface>())
				continue;

			for (auto SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if (SavedActor.ActorName == Actor->GetFName())
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}

					FMemoryReader MemoryReader(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);

					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAueaGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(
		Slot, 
		Maps.FindChecked(Slot->GetMapName())
	);
}

FString AAueaGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}

	return FString();
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
