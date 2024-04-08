// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DriftToGround(DeltaTime);
}

void ASoul::BeginPlay() {
	Super::BeginPlay();

	SetDriftingPath();
}


void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult) {

	IPickupInterface* Actor = Cast<IPickupInterface>(OtherActor);
	if (Actor) {
		Actor->AddSouls(this);

		SpawnPickupSystem();
		SpawnPickupSound();

		Destroy();
	}
}

void ASoul::SetDriftingPath() {
	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this, Start, End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true);

	DesiredZ = HitResult.ImpactPoint.Z + 50.f;
}

void ASoul::DriftToGround(float DeltaTime) {
	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ) {
		const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
		AddActorWorldOffset(DeltaLocation);
	}
}
