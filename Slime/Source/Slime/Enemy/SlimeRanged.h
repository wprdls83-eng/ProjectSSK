// SlimeRanged.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeRanged.generated.h"

class ASlimeEnemyProjectile;

UCLASS()
class SLIME_API ASlimeRanged : public ASlimeEnemy
{
	GENERATED_BODY()

public:
	ASlimeRanged();

protected:
	// 플레이어에게 원거리 공격을 시작할 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float AttackRange = 700.f;

	// 원거리 공격 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float AttackInterval = 2.f;

	// 원거리 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	float RangedDamage = 15.f;

	// 발사할 적 투사체 Blueprint 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged")
	TSubclassOf<ASlimeEnemyProjectile> ProjectileClass;

	// 공격 타이머
	FTimerHandle AttackTimerHandle;


protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 원거리 공격
	void RangedAttack();
};