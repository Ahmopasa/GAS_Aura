#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableActor.generated.h"


UCLASS()
class AURA_API ABreakableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UGeometryCollectionComponent> GeometryCollection;
};
