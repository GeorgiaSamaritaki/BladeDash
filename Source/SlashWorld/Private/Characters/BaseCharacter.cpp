
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter() {
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

}

void ABaseCharacter::Attack() {
}

void ABaseCharacter::Die() {
}

void ABaseCharacter::PlayAttackMontage() {
	if (AttackMontageSections.Num() <= 0) return;
	const int32 MaxSectionIndex = AttackMontageSections.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);

	PlayMontageSection(AttackMontage, AttackMontageSections[Selection]);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName) {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage) {
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint) {

	//Calculate the relative angle the enemy was hit to trigger the appropriate animation
	const FVector Forward = GetActorForwardVector();
	//Lower ImpactPoint to the enemy's actor location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal(); // Normalising vector

	// For the dot product (.)
	// Forward . ToHit = |Forward| |ToHit| * cosTheta 
	// Because normalised: Forward . ToHit = cosTheta
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta); // remember returns radiant
	Theta = FMath::RadiansToDegrees(Theta);

	//If crossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0) Theta *= -1;

	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f) {
		Section = FName("FromFront");
	} else if (Theta >= -135.f && Theta < -45.f) {
		Section = FName("FromLeft");
	} else if (Theta >= 45.f && Theta < 135.f) {
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint) {
	if (HitSound)
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint) {
	if (HitParticles && GetWorld())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
}

void ABaseCharacter::HandleDamage(float DamageAmount) {
	if (Attributes) {
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName) {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage) {
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}


bool ABaseCharacter::CanAttack() {
	return false;
}

bool ABaseCharacter::IsAlive() {
	return Attributes && Attributes->isAlive();
}

void ABaseCharacter::AttackEnd() {
}

void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) {
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox()) {
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}


	//Could be in weapon so we dont include boxcollision
}

