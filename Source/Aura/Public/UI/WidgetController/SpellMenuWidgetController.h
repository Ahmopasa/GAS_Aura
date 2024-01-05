#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AueaWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAueaWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature SpellPointsChanged;
};
