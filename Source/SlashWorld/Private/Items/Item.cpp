// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "SlashWorld/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"
#include "NiagaraComponent.h"

AItem::AItem() {
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	//Sphere component to track player's collision with the item
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay() {
	Super::BeginPlay();

	//Attach callbacks to sphere 
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

//Two functions to simulate a floating effect (we attach it in the blueprints to an axis)
float AItem::TransformedSin() {
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos() {
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

//Callback arguments taken from PrimitiveComponent
void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter) {
		SlashCharacter->SetOverlappingItem(this);
	}
}

//Callback arguments taken from PrimitiveComponent
void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter) {
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemStates::EIS_Hovering) {
		//Hovering with a sin func
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}
