// SlimeEnemyProjectile.cpp

#include "Slime/Enemy/SlimeEnemyProjectile.h"
#include "Slime/Character/SlimeCharacter.h"
#include "Slime/Character/SlimeEnemy.h"
#include "Slime/Player/SlimeMage.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"

ASlimeEnemyProjectile::ASlimeEnemyProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 일정 시간 뒤 자동 제거
	InitialLifeSpan = 5.f;

	// 충돌 컴포넌트 생성
	CollisionComponent =
		CreateDefaultSubobject<USphereComponent>(
			TEXT("CollisionComponent")
		);

	SetRootComponent(CollisionComponent);

	CollisionComponent->SetSphereRadius(20.f);

	CollisionComponent->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	CollisionComponent->SetCollisionResponseToAllChannels(
		ECR_Ignore
	);

	// Player Pawn과 겹치면 Overlap 발생
	CollisionComponent->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECR_Overlap
	);

	CollisionComponent->SetGenerateOverlapEvents(true);


	// 투사체 Mesh 생성
	ProjectileMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ProjectileMesh")
		);

	ProjectileMesh->SetupAttachment(CollisionComponent);

	ProjectileMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);
}

void ASlimeEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Overlap 함수 연결
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(
		this,
		&ASlimeEnemyProjectile::OnProjectileOverlap
	);
}

void ASlimeEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 이동 방향이 없다면 이동하지 않음
	if (MoveDirection.IsNearlyZero())
	{
		return;
	}

	// 발사할 때 저장한 방향으로만 직선 이동
	const FVector MoveDistance =
		MoveDirection * MoveSpeed * DeltaTime;

	AddActorWorldOffset(
		MoveDistance,
		true
	);
}

void ASlimeEnemyProjectile::SetMoveDirection(
	const FVector& NewDirection
)
{
	// 전달받은 방향 저장
	MoveDirection = NewDirection;

	// Z축 방향은 무시
	MoveDirection.Z = 0.f;

	// 방향 벡터 정규화
	MoveDirection.Normalize();

	// 이동 방향을 바라보도록 회전
	if (!MoveDirection.IsNearlyZero())
	{
		SetActorRotation(
			MoveDirection.Rotation()
		);
	}
}

void ASlimeEnemyProjectile::SetDamage(float NewDamage)
{
	// 음수 데미지 방지
	Damage = FMath::Max(
		0.f,
		NewDamage
	);
}

void ASlimeEnemyProjectile::OnProjectileOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// 반사된 Projectile이 Enemy와 충돌
	if (bIsReflected)
	{
		ASlimeEnemy* HitEnemy =
			Cast<ASlimeEnemy>(OtherActor);

		// Enemy가 아니라면 무시
		if (!IsValid(HitEnemy))
		{
			return;
		}

		// 죽은 Enemy라면 무시
		if (HitEnemy->IsDead())
		{
			return;
		}

		// 반사된 Projectile 데미지 적용
		HitEnemy->TakeDamageFromProjectile(
			Damage * ReflectDamageMultiplier
		);

		// 적중 후 Projectile 제거
		Destroy();

		return;
	}

	// 일반 Enemy Projectile
	ASlimeCharacter* PlayerCharacter =
		Cast<ASlimeCharacter>(OtherActor);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 마법사 반사 확인
	ASlimeMage* Mage =
		Cast<ASlimeMage>(PlayerCharacter);

	if (IsValid(Mage) && Mage->IsReflecting())
	{
		// 가장 가까운 Enemy 찾기
		ASlimeEnemy* TargetEnemy =
			FindNearestEnemy();

		// 반사할 Enemy가 없다면 Projectile 제거
		if (!IsValid(TargetEnemy))
		{
			Destroy();
			return;
		}

		// 반사 상태로 변경
		bIsReflected = true;

		// Projectile → Enemy 방향 계산
		FVector ReflectDirection =
			TargetEnemy->GetActorLocation()
			- GetActorLocation();

		ReflectDirection.Z = 0.f;
		ReflectDirection.Normalize();

		// 반사 방향으로 변경
		SetMoveDirection(
			ReflectDirection
		);

		// 마법사에게 데미지를 주지 않고 종료
		return;
	}

	// 일반 플레이어 피격
	PlayerCharacter->TakeDamageFromEnemy(
		Damage
	);

	// 플레이어에게 맞으면 Projectile 제거
	Destroy();
}

ASlimeEnemy* ASlimeEnemyProjectile::FindNearestEnemy() const
{
	// 현재 가장 가까운 Enemy
	ASlimeEnemy* NearestEnemy = nullptr;

	// 현재 가장 가까운 거리
	float NearestDistanceSquared =
		TNumericLimits<float>::Max();

	// 모든 Enemy 확인
	for (TActorIterator<ASlimeEnemy> It(GetWorld()); It; ++It)
	{
		ASlimeEnemy* Enemy = *It;

		// 유효하지 않은 Enemy는 제외
		if (!IsValid(Enemy))
		{
			continue;
		}

		// 죽은 Enemy는 제외
		if (Enemy->IsDead())
		{
			continue;
		}

		// Projectile과 Enemy 사이 거리 계산
		const float DistanceSquared =
			FVector::DistSquared2D(
				GetActorLocation(),
				Enemy->GetActorLocation()
			);

		// 현재보다 가까운 Enemy라면 갱신
		if (DistanceSquared < NearestDistanceSquared)
		{
			NearestDistanceSquared = DistanceSquared;
			NearestEnemy = Enemy;
		}
	}

	return NearestEnemy;
}

