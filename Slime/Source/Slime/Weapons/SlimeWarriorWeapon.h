// SlimeWarriorWeapon.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Weapons/SlimeWeaponBase.h"
#include "SlimeWarriorWeapon.generated.h"

UCLASS()
class SLIME_API ASlimeWarriorWeapon : public ASlimeWeaponBase
{
	GENERATED_BODY()

public:
	ASlimeWarriorWeapon();
	
protected:
	// 전사가 한 번 공격할 때 동시에 공격할 수 있는 최대 Enemy 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warrior|Weapon")
	int32 MaxHitEnemies = 3;

	// 최대 타격 Enemy 수 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warrior|Upgrade")
	int32 MaxHitEnemiesUpgradeAmount = 1;

	// 전방 공격 각도, 30이면 전사를 기준으로 좌우 15도씩 공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warrior|Weapon")
	float AttackAngle = 30.f;

	// 공격 각도 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Warrior|Upgrade")
	float AttackAngleUpgradeAmount = 5.f;

protected:
	// 전사 전용 근접 자동 공격
	virtual void AutoAttack() override;

	// 전사 실제 공격 범위를 디버그로 표시
	void DrawAttackDebug();

	// 공격 범위 안에서 가장 가까운 Enemy 찾기
	ASlimeEnemy* FindNearestEnemyInRange();

public:
	// 현재 최대 타격 수 반환
	int32 GetMaxHitEnemies() const { return MaxHitEnemies; }

	// 최대 타격 수 증가량 반환
	int32 GetMaxHitEnemiesUpgradeAmount() const { return MaxHitEnemiesUpgradeAmount; }

	// 현재 공격 각도 반환
	float GetAttackAngle() const { return AttackAngle; }

	// 공격 각도 증가량 반환
	float GetAttackAngleUpgradeAmount() const { return AttackAngleUpgradeAmount; }

	// 최대 타격 수 증가
	void UpgradeMaxHitEnemies();

	// 공격 각도 증가
	void UpgradeAttackAngle();
};
