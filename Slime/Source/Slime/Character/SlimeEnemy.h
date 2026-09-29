// SlimeEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlimeEnemy.generated.h"

class ASlimeExpOrbBase;
class USlimeDamageTextWidget;
class UMaterialInstanceDynamic;
class USoundBase;
class UNiagaraSystem;

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

	// Enemy 피격 시 생성할 데미지 Floating Text Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|UI")
	TSubclassOf<USlimeDamageTextWidget> DamageTextWidgetClass;

	// 피격 시 사용할 Dynamic Material
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> HitFlashMaterial;

	// Enemy 피격 시 재생할 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
	TObjectPtr<USoundBase> HitSound;

	// Enemy 사망 시 랜덤으로 재생할 사운드 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Sound")
	TArray<TObjectPtr<USoundBase>> DeathSounds;

	// Enemy 사망 시 재생할 Niagara 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Effect")
	TObjectPtr<UNiagaraSystem> DeathEffect;
	
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

	// 피격 Flash 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Hit")
	float HitFlashDuration = 0.1f;

	// 사망 후 실제로 제거되기까지의 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy|Death")
	float DeathDelay = 0.3f;

	// 현재 Wave의 난이도 배율을 Enemy 능력치에 적용
	void ApplyWaveStatMultiplier(
		float HealthMultiplier,
		float DamageMultiplier,
		float MoveSpeedMultiplier
	);

	// 현재 접촉 데미지를 줄 수 있는지
	bool bCanDealContactDamage = true;

	// Enemy가 이미 사망 처리되었는지 확인
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	bool bIsDead = false;

	// 접촉 데미지 쿨타임 Timer
	FTimerHandle ContactDamageTimerHandle;

	// 피격 시 원래 색으로 돌아오기 위한 Timer
	FTimerHandle HitFlashTimerHandle;

	// 사망 마무리 처리를 위한 Timer
	FTimerHandle DeathTimerHandle;

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

	// 피격 순간 시각 효과 시작
	void StartHitFlash();

	// 피격 효과 종료 후 원래 상태로 복구
	void EndHitFlash();

	// Enemy가 받은 데미지를 Floating Text로 표시
	void ShowDamageText(float DamageAmount);

	// 현재 Enemy가 접촉 시 적용할 데미지 반환
	virtual float GetContactDamage() const;

	// Enemy 사망 처리
	// 자식 Enemy가 각자 다른 사망 행동을 만들 수 있도록 virtual로 선언
	virtual void Die();

	// 실제 Enemy 사망 마무리 처리
	// EXP 드랍 + Wave 처치 알림 + Enemy 제거
	void FinishDeath();

	// 체력이 변경되었을 때 호출
	// 자식 Enemy가 필요한 추가 처리를 할 수 있도록 virtual로 구성
	virtual void OnHealthChanged();

public:
	// Projectile에게 피해를 받는 함수
	void TakeDamageFromProjectile(float DamageAmount);
	
	// 사망 여부를 반환하는 함수
	bool IsDead() const;
};
