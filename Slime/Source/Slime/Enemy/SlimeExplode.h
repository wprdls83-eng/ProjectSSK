// SlimeExplode.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeExplode.generated.h"

UCLASS()
class SLIME_API ASlimeExplode : public ASlimeEnemy
{
	GENERATED_BODY()
	
public:
	ASlimeExplode();

protected:
	// 자폭 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExplosionDamage = 35.f;

	// 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExplosionRadius = 300.f;

	// 폭발까지 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float ExplosionDelay = 3.f;

	// 플레이어에게 이 거리까지 접근하면 자폭 시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explode")
	float SelfDestructRange = 150.f;

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

	// 실제 폭발
	void Explode();
};
