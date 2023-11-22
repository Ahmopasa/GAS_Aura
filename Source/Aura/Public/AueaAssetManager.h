// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AueaAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAueaAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UAueaAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
