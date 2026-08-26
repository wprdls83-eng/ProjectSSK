// SlimeBoss.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeBoss.generated.h"

UCLASS()
class SLIME_API ASlimeBoss : public ASlimeEnemy
{
	GENERATED_BODY()

public:
	ASlimeBoss();

protected:
	// Boss 공격 가능 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	float BossAttackRange = 500.f;

	// Boss 공격 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	float BossAttackCooldown = 3.f;

	// 2페이즈가 시작되는 체력, 0.5 = 최대 체력의 50%
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	float PhaseTwoHealthPercent = 0.5f;

	// 2페이즈 공격 쿨타임 배율
	// 0.7 = 기존 쿨타임의 70%
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	float PhaseTwoAttackCooldownMultiplier = 0.7f;

	// 2페이즈 돌진 속도 배율
	// 1.3 = 기존 속도의 130%
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	float PhaseTwoChargeSpeedMultiplier = 1.3f;

	// 2페이즈 근접 공격 예고시간 배율
	// 0.7 = 기존 예고시간의 70%
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Phase")
	float PhaseTwoMeleeWarningMultiplier = 0.7f;

	// 현재 2페이즈에 진입했는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	bool bIsPhaseTwo = false;


// =========================
// Boss 근접 범위 공격
// =========================

	// Boss 근거리 범위 공격 반경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	float MeleeAttackRadius = 700.f;

	// Boss 근거리 범위 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	float MeleeAttackDamage = 25.f;

	// 근거리 공격이 실제로 발동되기 전 예고 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	float MeleeWarningTime = 3.f;

	// 현재 Boss가 공격 가능한 상태인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	bool bCanAttack = true;

	// 현재 Boss가 공격 동작 중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	bool bIsAttacking = false;

	// Boss 공격 쿨타임 Timer
	FTimerHandle BossAttackCooldownTimerHandle;

	// 근거리 공격 예고 Timer
	FTimerHandle MeleeWarningTimerHandle;

// =========================
// Boss 돌진 공격
// =========================

// 돌진 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Charge")
	float ChargeSpeed = 1500.f;

	// 돌진 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Charge")
	float ChargeDuration = 0.6f;

	// 돌진 전 예고 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Charge")
	float ChargeWarningTime = 0.8f;

	// 돌진 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Charge")
	float ChargeDamage = 35.f;

	// 돌진 공격의 좌우 위험 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Charge")
	float ChargeWarningWidth = 150.f;

	// 현재 돌진 중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Charge")
	bool bIsCharging = false;

	// 이번 돌진에서 플레이어에게 이미 데미지를 줬는지
	bool bHasDealtChargeDamage = false;

	// 돌진 시작 순간 저장한 방향
	FVector ChargeDirection = FVector::ZeroVector;

	// 돌진 예고 Timer
	FTimerHandle ChargeWarningTimerHandle;

	// 돌진 종료 Timer
	FTimerHandle ChargeDurationTimerHandle;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Boss 사망 처리
	virtual void Die() override;

	// Boss 접촉 데미지 반환
	// 돌진 중에는 일반 접촉 데미지를 막음
	virtual float GetContactDamage() const override;

	// Boss 체력이 변경되었을 때 호출
	virtual void OnHealthChanged() override;

	// 현재 체력을 확인하여 2페이즈 진입 여부 검사
	void CheckPhaseTwo();

	// Boss 2페이즈 시작
	void EnterPhaseTwo();


// =========================
// Boss 근접 범위 공격
// =========================

	// Boss 공격 가능 여부 확인
	void TryAttack();

	// Boss 공격 실행
	void PerformAttack();

	// 근거리 공격 예고 시작
	void PerformMeleeAttack();

	// 공격 쿨타임 종료
	void ResetAttackCooldown();

	// 예고 시간이 끝난 뒤 실제 공격 판정
	void ExecuteMeleeAttack();

// =========================
// Boss 돌진 공격
// =========================

	// 돌진 공격 준비
	void PerformChargeAttack();

	// 예고 시간이 끝난 후 실제 돌진 시작
	void StartCharge();

	// 돌진 종료
	void EndCharge();

	// Boss가 돌진 중 플레이어와 충돌했을 때 호출
	UFUNCTION()
	void OnBossChargeHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

public:
	// Boss 현재 체력 반환
	UFUNCTION(BlueprintPure, Category = "Boss|UI")
	float GetBossCurrentHealth() const;

	// Boss 최대 체력 반환
	UFUNCTION(BlueprintPure, Category = "Boss|UI")
	float GetBossMaxHealth() const;

	// Boss HP 비율 반환
	UFUNCTION(BlueprintPure, Category = "Boss|UI")
	float GetBossHealthPercent() const;
};
