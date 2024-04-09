// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PowerUp.h"
#include "Interfaces/PickupInterface.h"

void APowerUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult) {

	IPickupInterface* Actor = Cast<IPickupInterface>(OtherActor);
	if (Actor) {
		Actor->AddHealth(this);
		SpawnPickupSound();
		Destroy();

	}
}
