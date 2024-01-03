// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AueaCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AueaCharacter.generated.h"

class UNiagaraComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class AURA_API AAueaCharacter : public AAueaCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAueaCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState()override;

	//~begin Player Interface
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual int32 GetXP_Implementation() const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level)const;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints);
	virtual int32 GetSpellPointsReward_Implementation(int32 Level)const;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints);
	//~end Player Interface

	//~begin Combat Interface
	virtual int32 GetPlayerLevel_Implementation() override;
	//~end Combat Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom; 

	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
