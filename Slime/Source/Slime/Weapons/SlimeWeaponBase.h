// SlimeWeaponBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeWeaponBase.generated.h"

class ASlimeEnemy;
class ASlimeProjectile;

UCLASS()
class SLIME_API ASlimeWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlimeWeaponBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int WeaponLevel; // 무기 레밸
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int ProjectileCount; // 발사체 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Damage; // 무기 데미지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float AttackInterval; // 발사 간격 (공격 속도)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float AttackRange; // 공격 범위

	// 생성할 투사체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASlimeProjectile> ProjectileClass;

	// 자동 공격을 반복 실행하기 위한 타이머
	FTimerHandle AttackTimerHandle;

protected:
	virtual void BeginPlay() override;

	// 자동 공격 타이머를 시작하는 함수
	void StartAttackTimer();

	// 일정 시간마다 실행될 자동 공격 함수
	void AutoAttack();

	// 공격 범위 안에서 가장 가까운 적을 찾는 함수
	ASlimeEnemy* FindClosestEnemy() const;

	// 목표를 향해 투사체를 생성하는 함수
	void SpawnProjectile(ASlimeEnemy* TargetEnemy);
};
