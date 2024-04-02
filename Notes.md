# debug printing
```
int e = Avg<int32>(1, 3);
UE_LOG(LogTemp, Warning, TEXT("Avg of 1 and 3: %d"), e);

if (GEngine)
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
```
# 61
EditAnywhere -> Details panel
Blueprint -> In the blueprint schematics
BlueprintPure -> Doesnt have an execution node, good for functions that dont require input (Getters are pure)

```
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
```

# 63
```
//To avoid including header files in the .h for pointers, use class
//Creates an incomplete type
class ACreature : public APawn{
	class UCapsuleComponent* Capsule;
};
```

# 66
//Passing a Bind Axis to a pawn
```
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
}
```
## Enhanced input - Youtube Videos
Engine -> Input -> Default classes 
To add a module go to .Build.cs, to clean and reload the editor close editor, delete "Intermediate", "Saved" and "Binaries", 
right click on project "generate vs project files", rebuild "yes", open project and solution (there is also a refresh visual studio button in tools)


Old way (SetupPlayerInputComponent)
PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);

to view debug: showdebug enhancedinput

# 79 
```
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
```

# 86 
- Using Enhanced input actions
- State machines for Animations

![State machine](Screenshots/86.png)


# 87
- Inverse Kinematics in Animations
![Control rig](Screenshots/87.png)

# 90
Collisions: Physics and query in objects and pawns

For private variables, meta is needed to edit in blueprint:
```
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
```

# 92
Object Delegates

# 95
- `UFUNCTION()` macros need to be removed in inherited classes that overwrite the function
- Delegate functions are of `UFUNCTION()`

# 97 
Mixamo.com for animations
- Retargeting skeletal meshes 

# 100
inline keyword only works as a suggestion. Ue5 has FORCEINLINE

# 102 
Enum conventions
```
UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};
```

# 105 
Animation Graphs and simplifications
- Linked Anim Graph
- Blend Poses (Enum)

# 107
Animation Montages
- They have slots
- Slots can be added to ABPs to play 
- Notify when an attack ends and connect to a function

Good practices
- Refactor when possible
- Make const the var that are not changed

# 112
Adding sounds to animations
Sound cue
![Metasound](Screenshots/112.png)

# 116
Making sockets on skeletal meshes
Cueing Animation notification to call cpp functions

# 119
- Edit/Tweak animations using keys 

# 123
Tracing 
World locations CPP: `GetComponentLocation()` Blueprints: `GetWorldLocation()`
Relative location: CPP: `GetRelativeLocation()`
FHitResult 

# 128 
How to use interfaces for Unreal

# 133 
Root motion animations

# 134 - 137
For directional hit reactions:
- Dot product to determine direction of hit
- Unreal uses the left hand rule for the cross product
- Using `#include "Kismet/KismetSystemLibrary.h"` to draw vectors

# 138
Sound Attenuation

# 139
Visual effets in Cascade and Niagara

# 142 
Destructibles and how to fracture meshes

# 146
Blueprint Native events
When you set a function to also be edited in blueprints but with the ability 
to be overriden: `UFUNCTION(BlueprintNativeEvent)` this creates another (static) function
which is the one you overwrite called `funcName_Implementation(args)` (see getHit)

# 151
Instatioting Bluprint objects from cpp

# 152
Creating different kinds of treasures 
Niagara Particle Effects 

# 159
Variables can be binded to the editor using `UPROPERTY(meta = (BindWidget))` its important
that the cariable name is exactly the same (see HealthBar.h)

`UPROPERTY()` also makes sure varibles are not initialised with garbage data

# 161
`"Kismet/GameplayStatics.h"` contains the functions TakeDamage and Apply damage

# 165
Blueprint Thread Safe Update animations (access proxy with  `Property Access`)

# 168
AutopossessAI
Create and make dynamic nav mesh

# 170
When setting up AI controllers for enemies, for pathfinding to look accurate
Character Movement -> Orient Rotation to Movement -> True
Self -> Use Controller Rotation Yaw -> False

# 173
Enemy patrolling
Timer (Timer Handles)
Binding delegates to a function requires it to be UFUNCTION
Actor Tags

# 180
Origin change for weapons + sockets

# 186
Randomly select Montage section by setting an array  with the sections
![Montage Sections CPP](Screenshots/MontageSectionsCpp.png)
![Montage Sections Details Panel](Screenshots/MontageSectionsDetails.png)

# 197
- Motion warping for enemy attacks and dynamic combat

# 201 - 203
- Level Blueprint
- HUD overlay for character attributes