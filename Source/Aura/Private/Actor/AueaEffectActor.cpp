#include "Actor/AueaEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "Components/SphereComponent.h"

AAueaEffectActor::AAueaEffectActor()
{
 	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AAueaEffectActor::OnOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO : Change this to apply a Gameplay Effect.For now, using "const_cast". 
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor); ASCInterface) {
		const UAueaAttributeSet* AueaAttributeSet = Cast<UAueaAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAueaAttributeSet::StaticClass()));
		UAueaAttributeSet* MutableAueaAttributeSet = const_cast<UAueaAttributeSet*>(AueaAttributeSet);
		MutableAueaAttributeSet->SetHealth(AueaAttributeSet->GetHealth() + 25.f);
		MutableAueaAttributeSet->SetMana(AueaAttributeSet->GetMana() - 25.f);
		Destroy();
	}
}

void AAueaEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AAueaEffectActor::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAueaEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAueaEffectActor::EndOverlap);
}

