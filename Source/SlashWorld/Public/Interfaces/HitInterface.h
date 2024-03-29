// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
// Exposes interface to the reflection system of unreal
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface {
	GENERATED_BODY()
};

/**
 *
 */
class SLASHWORLD_API IHitInterface {
	GENERATED_BODY()

public:
	// its a pure virtual function
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint, AActor* Hitter);
};
