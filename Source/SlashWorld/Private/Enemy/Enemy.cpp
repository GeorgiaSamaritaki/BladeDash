// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

AEnemy::AEnemy() {

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	/* Movement */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	/* Sensing */
	SensingPawn = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	SensingPawn->SightRadius = 4000.f;
	SensingPawn->SetPeripheralVisionAngle(DefaultViewAngle);

	EnemyState = EEnemyState::EES_Patrolling;
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (IsDead()) return;

	// Checks enum to chasing or attacking
	if (EnemyState > EEnemyState::EES_Patrolling)
		CheckCombatTarget();
	else
		CheckPatrolTarget();

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {

	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if (IsInsideAttackRadious()) {
		EnemyState = EEnemyState::EES_Attacking;
	} else if (IsOutsideCombatRadious()) {
		ChaseTarget();
	}

	return DamageAmount;
}

void AEnemy::Destroyed() {
	if (EquippedWeapon) EquippedWeapon->Destroy();
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) {
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();

	StopAttackMontage();
	if (IsInsideCombatRadious() && !IsDead()) {
		StartAttackTimer();
	}
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();

	if (SensingPawn) SensingPawn->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	InitialiseEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::Die_Implementation() {
	Super::Die_Implementation();

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	DisableCapsule();
	HideHealthBar();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSoul();
}

void AEnemy::Attack() {
	Super::Attack();
	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack() {
	bool bCanAttack =
		IsInsideCombatRadious() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

void AEnemy::AttackEnd() {
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount) {
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget) {
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::SpawnDefaultWeapon() {
	UWorld* World = GetWorld();
	if (World && !WeaponSocketName.IsNone() && WeaponClass) {
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), WeaponSocketName, this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::SpawnSoul() {
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes) {
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
		if (SpawnedSoul) {
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::InitialiseEnemy() {
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::LoseInterest() {
	//Outside combat radius, lose interest
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::CheckCombatTarget() {
	if (IsOutsideCombatRadious()) {

		ClearAttackTimer();
		LoseInterest();

		if (!IsEngaged()) StartPatrolling();

	} else if (IsOutsideAttackRadious() && !IsChasing()) {

		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();

	} else if (CanAttack()) {
		StartAttackTimer();
	}
}

void AEnemy::StartAttackTimer() {
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer() {
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ChaseTarget() {
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::CheckPatrolTarget() {
	if (InTargetRange(PatrolTarget, PatrolRadius)) {

		PatrolTarget = ChoosePatrolTarget();
		StartPatrolTimer();
	}
}

AActor* AEnemy::ChoosePatrolTarget() {
	if (PatrolTargets.Num() > 0) {
		const int32 TargetSelection = FMath::RandRange(0, PatrolTargets.Num() - 1);
		return PatrolTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::StartPatrolling() {
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartPatrolTimer() {
	const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	SensingPawn->SetPeripheralVisionAngle(PatrolWaitViewAngle);
}

void AEnemy::ClearPatrolTimer() {
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	SensingPawn->SetPeripheralVisionAngle(DefaultViewAngle);
}


void AEnemy::PatrolTimerFinished() {
	MoveToTarget(PatrolTarget);
}

void AEnemy::MoveToTarget(AActor* Target) {
	if (EnemyController == nullptr && Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::PawnSeen(APawn* SeenPawn) {

	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&
		!SeenPawn->ActorHasTag(FName("Dead"))
		;

	if (bShouldChaseTarget) {
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius) {
	if (Target == nullptr) return false;

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	//DRAW_SPHERE_SingleFrame(GetActorLocation());
	//DRAW_SPHERE_SingleFrame(Target->GetActorLocation());
	return DistanceToTarget <= Radius;
}

void AEnemy::HideHealthBar() {
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar() {
	if (HealthBarWidget) {
		HealthBarWidget->SetVisibility(true);
	}
}

bool AEnemy::IsChasing() {
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking() {
	return EnemyState == EEnemyState::EES_Attacking;;
}

bool AEnemy::IsDead() {
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged() {
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsOutsideCombatRadious() {
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsInsideCombatRadious() {
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsOutsideAttackRadious() {
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadious() {
	return InTargetRange(CombatTarget, AttackRadius);
}
