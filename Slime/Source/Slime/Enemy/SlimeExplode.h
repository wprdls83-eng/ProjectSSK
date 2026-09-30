// SlimeExplode.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeExplode.generated.h"

class UNiagaraSystem;
class USoundBase;
class UAudioComponent;

UCLASS()
class SLIME_API ASlimeExplode : public ASlimeEnemy
{
	GENERATED_BODY()
	
public:
	ASlimeExplode();

protected:
	// 자폭 순간 재생할 Niagara 폭발 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explode|Effect")
	TObjectPtr<UNiagaraSystem> ExplosionEffect;

	// 자폭 준비 시작 시 재생할 알람 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explode|Sound")
	TObjectPtr<USoundBase> CountdownSound;

	// 폭발 순간 재생할 0초 알람 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explode|Sound")
	TObjectPtr<USoundBase> ZeroSecondSound;

	// 폭발 순간 재생할 폭발 사운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explode|Sound")
	TObjectPtr<USoundBase> ExplosionSound;

	// 현재 재생 중인 자폭 알람을 정지하기 위해 저장
	UPROPERTY()
	TObjectPtr<UAudioComponent> CountdownAudioComponent;

	// 자폭 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExplosionDamage = 35.f;

	// 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExplosionRadius = 300.f;

	// 폭발까지 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExplosionDelay = 1.5f;

	// 플레이어에게 이 거리까지 접근하면 자폭 시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float SelfDestructRange = 150.f;

	// 자폭 준비가 시작된 후 경과한 시간
	float SelfDestructElapsedTime = 0.f;

	// 이미 자폭 준비 중인지 확인
	bool bIsSelfDestructing = false;

	// 폭발 Timer
	FTimerHandle ExplosionTimerHandle;

protected:
	virtual void Tick(float DeltaTime) override;

	// 부모의 사망 처리 대신 자폭 시작
	virtual void Die() override;

	// 자폭 준비 시작
	void StartSelfDestruct();

	// 폭발준비
	void ExplodeReady();

	// 0초 알람 이후 실제 폭발 처리
	void FinalExplode();

	// 폭발 범위 경고 Mesh 표시/숨김을 Blueprint에서 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Explode|Warning")
	void SetExplosionWarningVisible(bool bVisible);
};
