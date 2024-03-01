// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AueaFireBall.h"

void AAueaFireBall::BeginPlay()
{
	Super::BeginPlay();
}

void AAueaFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
