// SlimeMageProjectile.cpp

#include "Slime/Weapons/SlimeMageProjectile.h"
#include "Slime/Character/SlimeEnemy.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASlimeMageProjectile::ASlimeMageProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	InitialLifeSpan = 5.f;

	// 충돌 컴포넌트 생성
	CollisionComponent =
		CreateDefaultSubobject<USphereComponent>(
			TEXT("CollisionComponent")
		);

	SetRootComponent(CollisionComponent);

	CollisionComponent->SetSphereRadius(20.f);

	// 현재는 실제 충돌 판정보다
	// 목표 위치 도착 여부로 폭발 처리
	CollisionComponent->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	// Mesh 생성
	ProjectileMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ProjectileMesh")
		);

	ProjectileMesh->SetupAttachment(
		CollisionComponent
	);

	ProjectileMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);
}

void ASlimeMageProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASlimeMageProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 위치
	const FVector CurrentLocation =
		GetActorLocation();

	// 아래 방향으로 이동
	const FVector NewLocation =
		CurrentLocation
		+ FVector(
			0.f,
			0.f,
			-FallSpeed * DeltaTime
		);

	SetActorLocation(NewLocation);

	// 목표 높이에 도달했다면 폭발
	if (GetActorLocation().Z <= TargetLocation.Z)
	{
		Explode();
	}
}

void ASlimeMageProjectile::SetTargetLocation(
	const FVector& NewTargetLocation
)
{
	TargetLocation = NewTargetLocation;
}

void ASlimeMageProjectile::SetDamage(
	float NewDamage
)
{
	Damage = FMath::Max(
		0.f,
		NewDamage
	);
}

void ASlimeMageProjectile::Explode()
{
	// 현재 폭발 위치
	const FVector ExplosionLocation =
		GetActorLocation();

	// 폭발 범위를 디버그 원으로 표시
	DrawDebugCircle(
		GetWorld(),
		ExplosionLocation + FVector(0.f, 0.f, 10.f),
		ExplosionRadius,
		64,
		FColor::Purple,
		false,
		1.f,
		0,
		3.f,
		FVector(1.f, 0.f, 0.f),
		FVector(0.f, 1.f, 0.f),
		false
	);

	// 월드에 존재하는 Enemy 검색
	TArray<AActor*> FoundEnemies;

	UGameplayStatics::GetAllActorsOfClass(
		this,
		ASlimeEnemy::StaticClass(),
		FoundEnemies
	);

	for (AActor* FoundActor : FoundEnemies)
	{
		ASlimeEnemy* Enemy =
			Cast<ASlimeEnemy>(FoundActor);

		if (!IsValid(Enemy))
		{
			continue;
		}

		if (Enemy->IsDead())
		{
			continue;
		}

		// 폭발 위치와 Enemy 거리 계산
		const float Distance =
			FVector::Dist2D(
				ExplosionLocation,
				Enemy->GetActorLocation()
			);

		// 폭발 범위 밖이면 무시
		if (Distance > ExplosionRadius)
		{
			continue;
		}

		// 범위 안의 모든 Enemy에게 데미지
		Enemy->TakeDamageFromProjectile(
			Damage
		);
	}

	// 폭발 후 Projectile 제거
	Destroy();
}

void ASlimeMageProjectile::SetExplosionRadius(float NewExplosionRadius)
{
	ExplosionRadius = FMath::Max(
		0.f,
		NewExplosionRadius
	);
}