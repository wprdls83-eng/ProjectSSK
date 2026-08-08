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
	// 적의 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float MaxHealth = 30.f;

	// 적의 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float CurrentHealth;

	// 플레이어에게 주는 기본 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Combat")
	float AttackDamage = 10.f;

	// 적을 처치했을 때 지급할 경험치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Exp")
	int32 ExpReward = 5;

	// 경험치 종류 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASlimeExpOrbBase> ExpOrbClass;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Projectile에게 피해를 받는 함수
	void TakeDamageFromProjectile(float DamageAmount);
};
