// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class SLASHWORLD_API ABreakableActor : public AActor, public IHitInterface {
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

	UFUNCTION()
	void OnBreak(const FChaosBreakEvent& BreakEvent);

private:
	// To gain accesss to the blueprints of the treasure class
	// Wrapper enforces restrictions
	UPROPERTY(EditAnywhere, Category = "Breakable Properties");
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	bool bBroken = false;
};
