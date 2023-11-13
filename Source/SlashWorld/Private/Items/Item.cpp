// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "SlashWorld/DebugMacros.h"

AItem::AItem() {
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay(){
	Super::BeginPlay();

	int e = Avg<int32>(1, 3);
	UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3: %d"), e);
}

float AItem::TransformedSin(){

	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos(){
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f)

	FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);
	DRAW_POINT_SingleFrame(AvgVector);
}
