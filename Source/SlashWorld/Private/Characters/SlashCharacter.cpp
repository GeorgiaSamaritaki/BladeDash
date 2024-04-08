
#include "Characters/SlashCharacter.h"

#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

ASlashCharacter::ASlashCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Declares the spring arm component and attaches it to the root component
	CameraBoom = CreateDefaultSubobject <USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	// Sets the length of the arm
	CameraBoom->TargetArmLength = 300.f;

	//Makes the Spring arm rorate with the Look() function
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	//Stops the camera from rotating with the controller. Only the spring arm.
	ViewCamera->bUsePawnControlRotation = false;

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::Tick(float DeltaTime) {
	if (Attributes) {
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::BeginPlay() {
	Super::BeginPlay();

	//Enhanced Input Controller
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) {
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));

	InitialiseSlashOverlay();


}

//Inputs are defined with the Enhanced input system (Blueprints/Characters/Input)
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	HandleDamage(DamageAmount);
	UpdateHUDHealth();
	return DamageAmount;
}


void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) {
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	CombatTarget = Hitter;
	if (Attributes && Attributes->GetHealthPercent() > 0.f) {
		UE_LOG(LogTemp, Warning, TEXT("hit reaction"));
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item) {
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul) {
	if (Attributes && SlashOverlay) {
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure) {
	if (Attributes && SlashOverlay) {
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value) {
	if (ActionState != EActionState::EAS_Unoccupied) return;

	if (Controller) {
		const FVector2D MovementVector = Value.Get<FVector2D>();

		/*const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementVector.Y);
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementVector.X);*/

		// For Controller input later in the course
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

}

void ASlashCharacter::Look(const FInputActionValue& Value) {
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller) {
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASlashCharacter::EKeyPressed() {
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon) {
		if (EquippedWeapon)
			EquippedWeapon->Destroy();

		EquipWeapon(OverlappingWeapon);

	} else if (EquippedWeapon) {
		if (CanDisarm()) Disarm();
		else if (CanArm()) Arm();
	}
}

void ASlashCharacter::Jump() {
	if (IsUnoccupied()) {
		Super::Jump();
	}
}
void ASlashCharacter::Dodge() {
	if (IsOccupied() || !HasEnoughStamina()) return;

	GetMesh()->SetGenerateOverlapEvents(false);
	ActionState = EActionState::EAS_Dodge;

	PlayDodgeMontage();

	if (Attributes && SlashOverlay) {
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::Attack() {
	Super::Attack();

	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
		CombatTarget = nullptr;

	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon) {
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	OverlappingItem = nullptr;
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	EquippedWeapon = Weapon;
}

void ASlashCharacter::AttackEnd() {
	Super::AttackEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd() {
	Super::DodgeEnd();
	GetMesh()->SetGenerateOverlapEvents(true);
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName) {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::Die_Implementation() {
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
}

bool ASlashCharacter::CanAttack() {
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm() {
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm() {
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Arm() {
	PlayEquipMontage(FName("Arm"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Disarm() {
	PlayEquipMontage(FName("Disarm"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttachWeaponToBack() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping() {
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd() {
	UE_LOG(LogTemp, Warning, TEXT("end hit react"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitialiseSlashOverlay() {
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD) {
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes) {
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

void ASlashCharacter::UpdateHUDHealth() {
	if (SlashOverlay && Attributes) {
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

bool ASlashCharacter::IsOccupied() {
	return ActionState != EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::IsUnoccupied() {
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::HasEnoughStamina() {
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}