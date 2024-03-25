// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

ASlashCharacter::ASlashCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);


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

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");


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

	Tags.Add(FName("SlashCharacter"));

}

void ASlashCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

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

void ASlashCharacter::Jump() {
	//if (IsUnoccupied()) { 
	Super::Jump();
	//}
}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) {
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox()) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}


	//Could be in weapon so we dont include boxcollision
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

		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		EquippedWeapon = OverlappingWeapon;
		OverlappingItem = nullptr;

	} else if (EquippedWeapon) {
		if (CanDisarm()) {
			PlayEquipMontage(FName("Disarm"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;

		} else if (CanArm()) {
			PlayEquipMontage(FName("Arm"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::Attack() {

	if (CanAttack()) {
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}


void ASlashCharacter::PlayAttackMontage() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage) {
		AnimInstance->Montage_Play(AttackMontage);

		//Pick Animation in Random
		const int32 Selection = FMath::RandRange(1, 3);
		FName SectionName = FName();
		switch (Selection) {
		case 1:
			SectionName = FName("Attack1");
			break;
		case 2:
			SectionName = FName("Attack2");
			break;
		case 3:
			SectionName = FName("Attack3");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName) {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::AttackEnd() {
	ActionState = EActionState::EAS_Unoccupied;
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

void ASlashCharacter::Disarm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm() {
	if (EquippedWeapon) {
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping() {
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Dodge() {
}