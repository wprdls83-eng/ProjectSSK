// SlimeEnemyProjectile.cpp

#include "Slime/Enemy/SlimeEnemyProjectile.h"
#include "Slime/Character/SlimeCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

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
	// 플레이어인지 확인
	ASlimeCharacter* PlayerCharacter =
		Cast<ASlimeCharacter>(OtherActor);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 기존 플레이어 피격 함수 재사용
	PlayerCharacter->TakeDamageFromEnemy(
		Damage
	);

	// 플레이어에게 맞으면 투사체 제거
	Destroy();
}

