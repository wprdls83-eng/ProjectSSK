// SlimeWarriorWeapon.cpp

#include "Slime/Weapons/SlimeWarriorWeapon.h"
#include "Slime/Character/SlimeEnemy.h"

#include "EngineUtils.h"

ASlimeWarriorWeapon::ASlimeWarriorWeapon()
{

}

void ASlimeWarriorWeapon::AutoAttack()
{	
	DrawAttackDebug();

	// 공격 범위 안에서 가장 가까운 Enemy 찾기
	ASlimeEnemy* TargetEnemy =
		FindNearestEnemyInRange();

	// 공격할 Enemy가 없다면 종료
	if (!IsValid(TargetEnemy))
	{
		return;
	}

	// 무기의 Owner인 플레이어 가져오기
	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return;
	}

	// 플레이어에서 Target Enemy로 향하는 방향
	FVector TargetDirection =
		TargetEnemy->GetActorLocation()
		- WeaponOwner->GetActorLocation();

	// 높이 차이 무시
	TargetDirection.Z = 0.f;

	// 방향 정규화
	TargetDirection.Normalize();

	// 현재 공격한 Enemy 수
	int32 HitEnemyCount = 0;

	// 공격 범위 제곱
	const float AttackRangeSquared =
		AttackRange * AttackRange;

	// 공격 각도의 절반
	const float HalfAttackAngle =
		AttackAngle * 0.5f;

	// 전방 판정에 사용할 최소 Dot 값
	const float MinimumDot =
		FMath::Cos(
			FMath::DegreesToRadians(HalfAttackAngle)
		);

	// 모든 Enemy 확인
	for (TActorIterator<ASlimeEnemy> It(GetWorld()); It; ++It)
	{
		ASlimeEnemy* Enemy = *It;

		if (!IsValid(Enemy))
		{
			continue;
		}

		if (Enemy->IsDead())
		{
			continue;
		}

		// 플레이어에서 Enemy로 향하는 벡터
		FVector DirectionToEnemy =
			Enemy->GetActorLocation()
			- WeaponOwner->GetActorLocation();

		DirectionToEnemy.Z = 0.f;

		// 거리 제곱
		const float DistanceSquared =
			DirectionToEnemy.SizeSquared();

		// 공격 범위 밖이면 무시
		if (DistanceSquared > AttackRangeSquared)
		{
			continue;
		}

		// 방향 벡터 정규화
		DirectionToEnemy.Normalize();

		// 플레이어 전방과 Enemy 방향의 내적
		const float Dot =
			FVector::DotProduct(
				WeaponOwner->GetActorForwardVector(),
				DirectionToEnemy
			);

		// 공격 각도 밖에 있는 Enemy라면 무시
		if (Dot < MinimumDot)
		{
			continue;
		}

		// 전방 공격 범위 안의 Enemy에게 데미지
		Enemy->TakeDamageFromProjectile(Damage);

		HitEnemyCount++;

		// 최대 타격 수에 도달했다면 종료
		if (HitEnemyCount >= MaxHitEnemies)
		{
			break;
		}
	}
}

ASlimeEnemy* ASlimeWarriorWeapon::FindNearestEnemyInRange()
{
	// 가장 가까운 Enemy
	ASlimeEnemy* NearestEnemy = nullptr;

	// 현재까지 발견한 가장 가까운 거리
	float NearestDistanceSquared =
		AttackRange * AttackRange;

	// 현재 무기 위치
	const FVector WeaponLocation =
		GetActorLocation();

	// 월드에 존재하는 모든 Enemy 확인
	for (TActorIterator<ASlimeEnemy> It(GetWorld()); It; ++It)
	{
		ASlimeEnemy* Enemy = *It;

		// 유효하지 않은 Enemy는 무시
		if (!IsValid(Enemy))
		{
			continue;
		}

		// 이미 죽은 Enemy는 무시
		if (Enemy->IsDead())
		{
			continue;
		}

		// Enemy와의 거리 계산
		const float DistanceSquared =
			FVector::DistSquared(
				WeaponLocation,
				Enemy->GetActorLocation()
			);

		// 현재까지 발견한 Enemy보다 가까우면 저장
		if (DistanceSquared <= NearestDistanceSquared)
		{
			NearestDistanceSquared = DistanceSquared;
			NearestEnemy = Enemy;
		}
	}

	return NearestEnemy;
}

void ASlimeWarriorWeapon::DrawAttackDebug()
{
	const FVector Center =
		GetActorLocation() + FVector(0.f, 0.f, 10.f);

	// 캐릭터가 바라보는 전방 방향
	const FVector Forward =
		GetActorForwardVector();

	// 공격 각도의 절반
	const float HalfAngle =
		AttackAngle * 0.5f;

	// 왼쪽 경계 방향
	const FVector LeftDirection =
		Forward.RotateAngleAxis(
			-HalfAngle,
			FVector::UpVector
		);

	// 오른쪽 경계 방향
	const FVector RightDirection =
		Forward.RotateAngleAxis(
			HalfAngle,
			FVector::UpVector
		);

	// 왼쪽 공격 경계선
	DrawDebugLine(
		GetWorld(),
		Center,
		Center + LeftDirection * AttackRange,
		FColor::Red,
		false,
		0.5f,
		0,
		3.f
	);

	// 오른쪽 공격 경계선
	DrawDebugLine(
		GetWorld(),
		Center,
		Center + RightDirection * AttackRange,
		FColor::Red,
		false,
		0.5f,
		0,
		3.f
	);

	// 공격 범위 호를 여러 선으로 나누어 표시
	const int32 SegmentCount = 20;

	FVector PreviousPoint =
		Center +
		LeftDirection * AttackRange;

	for (int32 i = 1; i <= SegmentCount; ++i)
	{
		const float Alpha =
			static_cast<float>(i) /
			static_cast<float>(SegmentCount);

		const float CurrentAngle =
			FMath::Lerp(
				-HalfAngle,
				HalfAngle,
				Alpha
			);

		const FVector CurrentDirection =
			Forward.RotateAngleAxis(
				CurrentAngle,
				FVector::UpVector
			);

		const FVector CurrentPoint =
			Center +
			CurrentDirection * AttackRange;

		DrawDebugLine(
			GetWorld(),
			PreviousPoint,
			CurrentPoint,
			FColor::Red,
			false,
			0.5f,
			0,
			3.f
		);

		PreviousPoint = CurrentPoint;
	}
}

void ASlimeWarriorWeapon::UpgradeMaxHitEnemies()
{
	MaxHitEnemies += MaxHitEnemiesUpgradeAmount;
}

void ASlimeWarriorWeapon::UpgradeAttackAngle()
{
	AttackAngle += AttackAngleUpgradeAmount;

	// 최대 360도 제한
	AttackAngle = FMath::Min(
		AttackAngle,
		360.f
	);
}