// SlimeMageWeapon.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Weapons/SlimeWeaponBase.h"
#include "SlimeMageWeapon.generated.h"

class ASlimeMageProjectile;

UCLASS()
class SLIME_API ASlimeMageWeapon : public ASlimeWeaponBase
{
	GENERATED_BODY()

public:
	ASlimeMageWeapon();

protected:
	// 마법사가 사용할 전용 Projectile 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mage|Weapon")
	TSubclassOf<ASlimeMageProjectile> MageProjectileClass;

	// Enemy 기준 Projectile 생성 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Weapon")
	float ProjectileSpawnHeight = 800.f;

	// 마법탄 기본 폭발 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mage|Weapon")
	float ExplosionRadius = 200.f;

	// 폭발 범위 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mage|Upgrade")
	float ExplosionRadiusUpgradeAmount = 50.f;

protected:
	// 마법사 전용 자동 공격
	virtual void AutoAttack() override;

public:
	// 현재 폭발 범위 반환
	float GetExplosionRadius() const { return ExplosionRadius; }

	// 폭발 범위 증가량 반환
	float GetExplosionRadiusUpgradeAmount() const { return ExplosionRadiusUpgradeAmount; }

	// 폭발 범위 증가
	void UpgradeExplosionRadius();
};
