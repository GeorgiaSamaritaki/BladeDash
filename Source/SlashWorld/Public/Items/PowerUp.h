// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "PowerUp.generated.h"

/**
 *
 */
UCLASS()
class SLASHWORLD_API APowerUp : public AItem {
	GENERATED_BODY()
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	int32 HP;
	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	int32 Stamina;

public:
	FORCEINLINE int32 GetHealth() const { return HP; }
	FORCEINLINE int32 GetStamina() const { return Stamina; }
};
