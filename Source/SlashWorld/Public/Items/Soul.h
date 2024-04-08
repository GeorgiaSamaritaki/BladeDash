// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

UCLASS()
class SLASHWORLD_API ASoul : public AItem {
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult) override;

private:
	void SetDriftingPath();
	void DriftToGround(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls = 1;

	double DesiredZ;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float DriftRate = -30.f;
public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
};
