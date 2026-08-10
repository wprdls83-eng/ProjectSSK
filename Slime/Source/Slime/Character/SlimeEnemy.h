// SlimeEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlimeEnemy.generated.h"

class ASlimeExpOrbBase;

UCLASS()
class SLIME_API ASlimeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ASlimeEnemy();

protected:
	// 적을 처치했을 때 지급할 경험치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Exp")
	int32 ExpReward = 5;

	// 경험치 종류 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASlimeExpOrbBase> ExpOrbClass;
	
public:
	// 적의 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float MaxHealth = 30.f;

	// 적의 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float CurrentHealth;

	// 플레이어와 접촉했을 때 입힐 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float AttackDamage = 10.f;

	// 접촉 데미지 재적용 대기시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Attack")
	float ContactDamageCooldown = 1.f;

	// 현재 접촉 데미지를 줄 수 있는지
	bool bCanDealContactDamage = true;

	// Enemy가 이미 사망 처리되었는지 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	bool bIsDead = false;

	// 접촉 데미지 쿨타임 Timer
	FTimerHandle ContactDamageTimerHandle;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 플레이어와 충돌했을 때 호출
	UFUNCTION()
	void OnEnemyHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

	// 접촉 데미지 쿨타임 종료
	void ResetContactDamage();

	// 현재 Enemy가 접촉 시 적용할 데미지 반환
	virtual float GetContactDamage() const;

	// Enemy 사망 처리
	// 자식 Enemy가 각자 다른 사망 행동을 만들 수 있도록 virtual로 선언
	virtual void Die();

	// 실제 Enemy 사망 마무리 처리
	// EXP 드랍 + Wave 처치 알림 + Enemy 제거
	void FinishDeath();
public:
	// Projectile에게 피해를 받는 함수
	void TakeDamageFromProjectile(float DamageAmount);
	
	// 사망 여부를 반환하는 함수
	bool IsDead() const;
};
