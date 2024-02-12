AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
UE_LOG(LogTemp, Warning, TEXT("%f"), value);

# 61
EditAnywhere -> Details panel
Blueprint -> In the blueprint schematics
BlueprintPure -> Doesnt have an execution node, good for functions that dont require input (Getters are pure)

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

# 66
//Passing a Bind Axis to a pawn

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
}

## Enhanced input - Youtube Videos
Engine -> Input -> Default classes 
To add a module go to .Build.cs, to clean and reload the editor close editor, delete "Intermediate", "Saved" and "Binaries", 
right click on project "generate vs project files", rebuild "yes", open project and solution (there is also a refresh visual studio button in tools)


Old way (SetupPlayerInputComponent)
PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);

to view debug: showdebug enhancedinput

# 79 
//Declares the spring arm component and attaches it to the root component
	CameraBoom = CreateDefaultSubobject < USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	// Sets the length of the arm
	CameraBoom->TargetArmLength = 300.f;

	//Makes the Spring arm rorate with the Look() function
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Stops the camera from rotating with the controller. Only the spring arm.
	ViewCamera->bUsePawnControlRotation = false;

# 86 
Using Enhanced input actions
