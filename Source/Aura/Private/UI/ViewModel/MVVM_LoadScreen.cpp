#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include <Kismet/GameplayStatics.h>
#include "Game/AueaGameModeBase.h"
#include <Game/AueaGameInstance.h>

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);

	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));

	LoadSlot_0->SlotIndex = 0;
	LoadSlot_1->SlotIndex = 1;
	LoadSlot_2->SlotIndex = 2;

	LoadSlots.Add(0, LoadSlot_0);
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnteredName)
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AueaGameMode))
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Magenta, FString("Please, switch to Single Player"));
	}

	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->SetMapName(AueaGameMode->DefaultMapName);
	LoadSlots[Slot]->SlotStatus = Taken;
	LoadSlots[Slot]->PlayerStartTag = AueaGameMode->DefaultPlayerStartTag;
	LoadSlots[Slot]->MapAssetName = AueaGameMode->DefaultMap.ToSoftObjectPath().GetAssetName();

	AueaGameMode->SaveSlotData(LoadSlots[Slot], Slot);

	LoadSlots[Slot]->InitializeSlot();

	auto* AueaGameInstance = Cast<UAueaGameInstance>(AueaGameMode->GetGameInstance());
	AueaGameInstance->LoadSlotName = LoadSlots[Slot]->GetLoadSlotName();
	AueaGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex;
	AueaGameInstance->PlayerStartTag = AueaGameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();

	for (const auto& LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		else
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
	}

	SelectedSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::YesButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AAueaGameModeBase::DeleteSlot(SelectedSlot->GetLoadSlotName(), SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));
	auto* AueaGameInstance = Cast<UAueaGameInstance>(AueaGameMode->GetGameInstance());
	AueaGameInstance->PlayerStartTag = SelectedSlot->PlayerStartTag;
	AueaGameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();
	AueaGameInstance->LoadSlotIndex = SelectedSlot->SlotIndex;
	if (IsValid(SelectedSlot))
	{
		AueaGameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));

	if (!IsValid(AueaGameMode)) return;

	for (const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveObject = AueaGameMode->GetSaveSlotData(LoadSlot.Value->GetLoadSlotName(), LoadSlot.Key);

		const FString PlayerName = SaveObject->PlayerName;
		TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveObject->SaveSlotStatus;

		LoadSlot.Value->SlotStatus = SaveSlotStatus;
		LoadSlot.Value->SetPlayerName(PlayerName);
		LoadSlot.Value->InitializeSlot();

		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->PlayerStartTag = SaveObject->PlayerStartTag;
		LoadSlot.Value->SetPlayerLevel(SaveObject->PlayerLevel);
	}
}
