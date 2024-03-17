#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include <Kismet/GameplayStatics.h>
#include "Game/AueaGameModeBase.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);

	LoadSlot_0->LoadSlotName = FString("LoadSlot_0");
	LoadSlot_1->LoadSlotName = FString("LoadSlot_1");
	LoadSlot_2->LoadSlotName = FString("LoadSlot_2");

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

	LoadSlots[Slot]->SetPlayerName(EnteredName);
	LoadSlots[Slot]->SetMapName(AueaGameMode->DefaultMapName);
	LoadSlots[Slot]->SlotStatus = Taken;

	AueaGameMode->SaveSlotData(LoadSlots[Slot], Slot);

	LoadSlots[Slot]->InitializeSlot();
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
		AAueaGameModeBase::DeleteSlot(SelectedSlot->LoadSlotName, SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus = Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (SelectedSlot)
	{
		AueaGameMode->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	auto* AueaGameMode = Cast<AAueaGameModeBase>(UGameplayStatics::GetGameMode(this));

	for (const auto& LoadSlot : LoadSlots)
	{
		auto* SaveObject = AueaGameMode->GetSaveSlotData(LoadSlot.Value->LoadSlotName, LoadSlot.Key);
	
		LoadSlot.Value->SetPlayerName(SaveObject->PlayerName);
		LoadSlot.Value->SetMapName(SaveObject->MapName);
		LoadSlot.Value->SlotStatus = SaveObject->SaveSlotStatus;
		LoadSlot.Value->InitializeSlot();
	}
}
