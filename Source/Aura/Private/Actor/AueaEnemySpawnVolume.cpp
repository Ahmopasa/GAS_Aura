#include "Actor/AueaEnemySpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Actor/AueaEnemySpawnPoint.h"


AAueaEnemySpawnVolume::AAueaEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void AAueaEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
		Destroy();
}

// Called when the game starts or when spawned
void AAueaEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAueaEnemySpawnVolume::OnBoxOverlap);
}

void AAueaEnemySpawnVolume::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<class UPlayerInterface>()) return;

	bReached = true;

	for (auto* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}

	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}



