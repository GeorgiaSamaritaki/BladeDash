// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "AIController.h"
#include "Items/Weapons/Weapon.h"

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
	SensingPawn->SetPeripheralVisionAngle(45.f);

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
	ChaseTarget();

	return DamageAmount;
}

void AEnemy::Destroyed() {
	if (EquippedWeapon) EquippedWeapon->Destroy();
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint) {

	ShowHealthBar();

	if (IsAlive()) DirectionalHitReact(ImpactPoint);
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();

	if (SensingPawn) SensingPawn->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitialiseEnemy();
}

void AEnemy::Die() {
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	DisableCapsule();
	HideHealthBar();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AEnemy::Attack() {
	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
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

int32 AEnemy::PlayDeathMontage() {
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX) {
		DeathPose = Pose;
	}
	return Selection;
}

void AEnemy::SpawnDefaultWeapon() {
	UWorld* World = GetWorld();
	if (World) {
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
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
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
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

void AEnemy::ClearPatrolTimer() {
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::PatrolTimerFinished() {
	MoveToTarget(PatrolTarget);
}

void AEnemy::MoveToTarget(AActor* Target) {
	if (EnemyController == nullptr && Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);

	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::PawnSeen(APawn* SeenPawn) {

	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("SlashCharacter"))
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
