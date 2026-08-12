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
	// 발사체 수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int ProjectileCount; 

	// 무기 데미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float Damage; 

	// 발사 간격 (공격 속도)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float AttackInterval; 

	// 공격 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float AttackRange;

	// 레벨업 1회당 공격력 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Upgrade")
	float DamageUpgradeAmount = 5.f;

	// 레벨업 1회당 공격 간격 감소량, AttackInterval이 작을수록 더 빠르게 공격
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Upgrade")
	float AttackIntervalUpgradeAmount = 0.1f;

	// 레벨업 1회당 공격 범위 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Upgrade")
	float AttackRangeUpgradeAmount = 50.f;

	// 발사체 수 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Upgrade")
	int32 ProjectileCountUpgradeAmount = 1;

	// 생성할 투사체 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASlimeProjectile> ProjectileClass;

	// 자동 공격을 반복 실행하기 위한 타이머
	FTimerHandle AttackTimerHandle;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 자동 공격 타이머를 시작하는 함수
	void StartAttackTimer();

	// 일정 시간마다 실행될 자동 공격 함수
	virtual void AutoAttack();

	// 공격 범위 안에서 가장 가까운 적을 찾는 함수
	ASlimeEnemy* FindClosestEnemy() const;

	// 목표를 향해 투사체를 생성하는 함수
	void SpawnProjectile(ASlimeEnemy* TargetEnemy, float SideOffset = 0.f);

public:
	// 공격 중지 함수
	void StopAttackTimer();

	// 무기 데미지 증가
	void UpgradeDamage();

	// 공격 속도 증가
	void UpgradeAttackSpeed();

	// 발사체 개수 증가
	void UpgradeProjectileCount();

	// 공격 범위 업그레이드
	void UpgradeAttackRange();

	// 현재 무기 공격력 반환
	float GetDamage() const { return Damage; }

	// 공격력 업그레이드 증가량 반환
	float GetDamageUpgradeAmount() const { return DamageUpgradeAmount; }

	// 현재 공격 간격 반환
	float GetAttackInterval() const { return AttackInterval; }

	// 공격 간격 업그레이드 감소량 반환
	float GetAttackIntervalUpgradeAmount() const { return AttackIntervalUpgradeAmount; }

	// 현재 공격 범위 반환
	float GetAttackRange() const { return AttackRange; }

	// 공격 범위 업그레이드 증가량 반환
	float GetAttackRangeUpgradeAmount() const { return AttackRangeUpgradeAmount; }

	// 현재 발사체 수 반환
	int32 GetProjectileCount() const { return ProjectileCount; }

	// 발사체 수 증가량 반환
	int32 GetProjectileCountUpgradeAmount() const { return ProjectileCountUpgradeAmount; }
};
