AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
UE_LOG(LogTemp, Warning, TEXT("%f"), value);

# 61
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UPROPERTY(EditBlueprint, BlueprintReadWrite) 
    float RotationRate = 0.25f;

	UFUNCTION(BlueprintRead) 
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();




# 63
//To avoid including header files in the .h for pointers, use class
//Creates an incomplete type
class ACreature : public APawn{
	class UCapsuleComponent* Capsule;
};

#66
//Passing a Bind Axis to a pawn

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
}

