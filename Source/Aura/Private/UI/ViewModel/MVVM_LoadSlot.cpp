#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	//const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	//SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
	
	SetWidgetSwitcherIndex.Broadcast(SlotStatus.GetValue());
}

void UMVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

FString UMVVM_LoadSlot::GetPlayerName() const
{
	return PlayerName;
}
