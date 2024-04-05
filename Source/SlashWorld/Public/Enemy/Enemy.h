// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class SLASHWORLD_API AEnemy : public ABaseCharacter {
	GENERATED_BODY()
public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState;

private:
	void SpawnDefaultWeapon();
	void SpawnSoul();
	void InitialiseEnemy();

	/** AI Behavior */
	void LoseInterest();

	void CheckCombatTarget();
	void StartAttackTimer();
	void ClearAttackTimer();

	void ChaseTarget();
	void CheckPatrolTarget();
	AActor* ChoosePatrolTarget();
	void StartPatrolling();
	void StartPatrolTimer();
	void ClearPatrolTimer();
	void PatrolTimerFinished();
	void MoveToTarget(AActor* Target);

	UFUNCTION() //Callback for OnPawnSeen in UPawnSensingComponent
		void PawnSeen(APawn* SeenPawn);

	bool InTargetRange(AActor* Target, double Radius);
	void HideHealthBar();
	void ShowHealthBar();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	bool IsOutsideCombatRadious();
	bool IsInsideCombatRadious();
	bool IsOutsideAttackRadious();
	bool IsInsideAttackRadious();

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UPawnSensingComponent* SensingPawn;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponSocketName = FName();

	UPROPERTY(EditAnywhere, Category = "Combat")
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	double AcceptanceRadius = 50.f;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget; //Current Patrol Target

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	// A little larger than moveto uses to make sure it triggers
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitViewAngle = 90.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float DefaultViewAngle = 45.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 300.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ASoul> SoulClass;
};
