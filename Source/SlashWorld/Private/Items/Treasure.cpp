// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interfaces/PickupInterface.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	IPickupInterface* Actor = Cast<IPickupInterface>(OtherActor);
	if (Actor) {
		Actor->AddGold(this);
	}

	SpawnPickupSound();
	Destroy();

}
