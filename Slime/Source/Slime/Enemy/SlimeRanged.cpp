// SlimeRanged.cpp

#include "Slime/Enemy/SlimeRanged.h"
#include "Slime/Enemy/SlimeEnemyProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

ASlimeRanged::ASlimeRanged()
{
}

void ASlimeRanged::BeginPlay()
{
	Super::BeginPlay();

	// 일정 시간마다 원거리 공격 시도
	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ASlimeRanged::RangedAttack,
		AttackInterval,
		true
	);
}

void ASlimeRanged::Tick(float DeltaTime)
{
	// 플레이어 가져오기
	ACharacter* PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 플레이어와의 거리 계산
	const float DistanceToPlayer =
		FVector::Dist2D(
			GetActorLocation(),
			PlayerCharacter->GetActorLocation()
		);

	// 공격 범위 밖이라면
	if (DistanceToPlayer > AttackRange)
	{
		// 부모 SlimeEnemy의 추적 로직 실행
		Super::Tick(DeltaTime);
	}
	else
	{
		// 공격 범위 안이라면 이동 정지
		GetCharacterMovement()->StopMovementImmediately();

		// 플레이어 방향 계산
		FVector LookDirection =
			PlayerCharacter->GetActorLocation() - GetActorLocation();

		LookDirection.Z = 0.f;

		// 플레이어를 바라보도록 회전
		if (!LookDirection.IsNearlyZero())
		{
			SetActorRotation(
				LookDirection.Rotation()
			);
		}
	}
}

void ASlimeRanged::RangedAttack()
{
	// 투사체 클래스가 설정되지 않았다면 종료
	if (!ProjectileClass)
	{
		return;
	}

	// 플레이어 가져오기
	ACharacter* PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 플레이어와 거리 계산
	const float DistanceToPlayer =
		FVector::Dist2D(
			GetActorLocation(),
			PlayerCharacter->GetActorLocation()
		);

	// 공격 범위 밖이라면 발사하지 않음
	if (DistanceToPlayer > AttackRange)
	{
		return;
	}

	// 발사 위치
	const FVector SpawnLocation =
		GetActorLocation();

	// 발사 순간 플레이어 방향 계산
	FVector FireDirection =
		PlayerCharacter->GetActorLocation()
		- SpawnLocation;

	FireDirection.Z = 0.f;
	FireDirection.Normalize();

	// 방향을 회전값으로 변환
	const FRotator SpawnRotation =
		FireDirection.Rotation();

	// 적 투사체 생성
	ASlimeEnemyProjectile* SpawnedProjectile =
		GetWorld()->SpawnActor<ASlimeEnemyProjectile>(
			ProjectileClass,
			SpawnLocation,
			SpawnRotation
		);

	if (!IsValid(SpawnedProjectile))
	{
		return;
	}

	// 발사 순간 방향 고정
	SpawnedProjectile->SetMoveDirection(
		FireDirection
	);

	// 데미지 전달
	SpawnedProjectile->SetDamage(
		RangedDamage
	);
}