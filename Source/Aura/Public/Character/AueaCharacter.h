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
	virtual int32 GetAttributePointsReward_Implementation(int32 Level)const override;
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level)const override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void ShowMagicCircle_Implementation(UMaterialInstance* DecalMaterial) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;
	//~end Player Interface

	//~begin Combat Interface
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override; 
	//~end Combat Interface

	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;

	FTimerHandle DeathTimer; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	virtual void OnRep_Stunned() override;

	virtual void OnRep_Burned() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> CameraBoom;

	void LoadProgress();
private:
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
