// SlimeRanged.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeRanged.generated.h"

class ASlimeEnemyProjectile;
class UNiagaraSystem;

UCLASS()
class SLIME_API ASlimeRanged : public ASlimeEnemy
{
	GENERATED_BODY()

public:
	ASlimeRanged();

protected:
	// 플레이어에게 원거리 공격을 시작할 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float AttackRange = 1000.f;

	// 원거리 공격 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float AttackInterval = 5.f;

	// 원거리 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float RangedDamage = 15.f;

	// 발사할 적 투사체 Blueprint 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	TSubclassOf<ASlimeEnemyProjectile> ProjectileClass;

	// 원거리 공격 발사 순간 재생할 Niagara 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged|Effect")
	TObjectPtr<UNiagaraSystem> FireEffect;

	// 공격 타이머
	FTimerHandle AttackTimerHandle;


protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 원거리 공격
	void RangedAttack();
};