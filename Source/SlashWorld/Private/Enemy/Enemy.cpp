// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SlashWorld/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"

AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	/* Movement */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();
	if (HealthBarWidget) HealthBarWidget->SetVisibility(false);

	EnemyController = Cast<AAIController>(GetController());

	if (EnemyController && PatrolTarget) {
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(PatrolTarget);
		MoveRequest.SetAcceptanceRadius(15.f);
		FNavPathSharedPtr NavPath;
		EnemyController->MoveTo(MoveRequest, &NavPath);
		TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();

		for (auto& Point : PathPoints) {
			const FVector& Location = Point.Location;
			DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Green, false, 10.f);
		}
	}
}

void AEnemy::Die() {
	PlayRandomDeathMontage();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HealthBarWidget) HealthBarWidget->SetVisibility(false);
	SetLifeSpan(3.f);
}

void AEnemy::PlayRandomDeathMontage() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && DeathMontage) {
		AnimInstance->Montage_Play(DeathMontage);

		FName SectionName = FName();
		const int32 Selection = FMath::RandRange(1, 5);
		switch (Selection) {
		case 1:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Dead1;
			break;
		case 2:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Dead2;
			break;
		case 3:
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Dead3;
			break;
		case 4:
			SectionName = FName("Death4");
			DeathPose = EDeathPose::EDP_Dead4;
			break;
		case 5:
			SectionName = FName("Death5");
			DeathPose = EDeathPose::EDP_Dead5;
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius) {
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::PlayHitReactMontage(const FName& SectionName) {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage) {
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (CombatTarget) {
		if (!InTargetRange(CombatTarget, CombatRadius)) {
			CombatTarget = nullptr;
			if (HealthBarWidget)
				HealthBarWidget->SetVisibility(false);
		}
	}
	if (EnemyController && PatrolTarget) {
		if (InTargetRange(PatrolTarget, PatrolRadius) && PatrolTargets.Num() > 1) {

			const int32 TargetSelection = FMath::RandRange(0, PatrolTargets.Num() - 1);
			PatrolTarget = PatrolTargets[TargetSelection];

			FAIMoveRequest MoveRequest;
			MoveRequest.SetGoalActor(PatrolTarget);
			MoveRequest.SetAcceptanceRadius(15.f);
			EnemyController->MoveTo(MoveRequest);

		}

	}

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint) {
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}

	if (Attributes && Attributes->isAlive()) {
		DirectionalHitReact(ImpactPoint);
	} else {
		Die();
	}

	if (HitSound)
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);

	if (HitParticles && GetWorld())
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
}

// Determines which animation to play based on where the actor was hit
// Calls PlayHitReactMontage for the selection
void AEnemy::DirectionalHitReact(const FVector& ImpactPoint) {

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

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));

	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {

	if (Attributes && HealthBarWidget) {
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	CombatTarget = EventInstigator->GetPawn();

	return DamageAmount;
}

