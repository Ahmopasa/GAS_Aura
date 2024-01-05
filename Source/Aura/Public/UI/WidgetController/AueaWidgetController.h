#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AueaWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAueaAbilityInfo&, Info);

class UAbilitySystemComponent;
class UAttributeSet;
class AAueaPlayerController;
class AAueaPlayerState;
class UAueaAbilitySystemComponent;
class UAueaAttributeSet;
class UAbilityInfo;

USTRUCT(BlueprintType)
struct FWidgetControllerParams {
	GENERATED_BODY()

	FWidgetControllerParams() {}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;

};

UCLASS()
class AURA_API UAueaWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();

	virtual void BindCallbacksToDependencies();

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	void BroadcastAbilityInfo();

protected:
	// ~begin Base
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
	// ~end Base

	// ~begin Auea
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAueaPlayerController> AueaPlayerController;
	AAueaPlayerController* GetAueaPC();
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AAueaPlayerState> AueaPlayerState;
	AAueaPlayerState* GetAueaPS();
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAueaAbilitySystemComponent> AueaAbilitySystemComponent;
	UAueaAbilitySystemComponent* GetAueaASC();
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAueaAttributeSet> AueaAttributeSet;
	UAueaAttributeSet* GetAueaAS();
	// ~end Auea
};
