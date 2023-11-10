// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "SlashWorld/DebugMacros.h"

AItem::AItem(){
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay(){
	Super::BeginPlay();

}

void AItem::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	float MovementRate = 50.f;
	float RotationRate = 45.f;

	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	AddActorWorldRotation(FRotator(0.f, RotationRate* DeltaTime, 0.f));

	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f)
}

