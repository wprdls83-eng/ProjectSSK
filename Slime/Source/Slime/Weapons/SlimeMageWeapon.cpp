// SlimeMageWeapon.cpp

#include "Slime/Weapons/SlimeMageWeapon.h"
#include "Slime/Weapons/SlimeMageProjectile.h"
#include "Slime/Character/SlimeEnemy.h"


ASlimeMageWeapon::ASlimeMageWeapon()
{

}

void ASlimeMageWeapon::AutoAttack()
{
	// 공격 범위 안에서 가장 가까운 Enemy 찾기
	ASlimeEnemy* TargetEnemy =
		FindClosestEnemy();

	// 공격할 Enemy가 없다면 종료
	if (!IsValid(TargetEnemy))
	{
		return;
	}

	// 마법사 전용 Projectile 클래스가 없다면 생성 불가
	if (!MageProjectileClass)
	{
		return;
	}

	// Enemy의 현재 위치를 공격 지점으로 저장
	FVector TargetLocation =
		TargetEnemy->GetActorLocation();

	// Projectile 생성 위치
	// Enemy의 머리 위쪽에서 생성
	FVector SpawnLocation =
		TargetLocation;

	SpawnLocation.Z += ProjectileSpawnHeight;

	// 마법 Projectile 생성
	ASlimeMageProjectile* MageProjectile =
		GetWorld()->SpawnActor<ASlimeMageProjectile>(
			MageProjectileClass,
			SpawnLocation,
			FRotator::ZeroRotator
		);

	// 생성 실패
	if (!IsValid(MageProjectile))
	{
		return;
	}

	// Projectile에게 떨어질 위치 전달
	MageProjectile->SetTargetLocation(
		TargetLocation
	);

	// Weapon의 현재 Damage 전달
	MageProjectile->SetDamage(
		Damage
	);

	// 현재 마법사 무기의 폭발 범위를 Projectile에게 전달
	MageProjectile->SetExplosionRadius(
		ExplosionRadius
	);
}

void ASlimeMageWeapon::UpgradeExplosionRadius()
{
	ExplosionRadius +=
		ExplosionRadiusUpgradeAmount;
}
