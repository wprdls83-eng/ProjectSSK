// SlimeProjectile.cpp

#include "Slime/Weapons/SlimeProjectile.h"
#include "Slime/Character/SlimeEnemy.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ASlimeProjectile::ASlimeProjectile()
{

	PrimaryActorTick.bCanEverTick = true;

    InitialLifeSpan = 3.f;

    // 충돌 컴포넌트 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(
        TEXT("CollisionComponent")
    );

    // 충돌 컴포넌트를 루트로 지정
    SetRootComponent(CollisionComponent);

    // 충돌 구체 크기 설정
    CollisionComponent->SetSphereRadius(20.f);

    // 충돌 감지만 사용
    CollisionComponent->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );

    // 모든 채널은 우선 무시
    CollisionComponent->SetCollisionResponseToAllChannels(
        ECR_Ignore
    );

    // Pawn과 겹치면 Overlap 발생
    CollisionComponent->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Overlap
    );

    // Overlap 이벤트 활성화
    CollisionComponent->SetGenerateOverlapEvents(true);

    // 투사체 메시 생성
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(
        TEXT("ProjectileMesh")
    );

    // 투사체 메시를 충돌 컴포넌트에 부착
    ProjectileMesh->SetupAttachment(CollisionComponent);

    // 메시 자체의 충돌은 비활성화
    CollisionComponent->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );

    ProjectileMesh->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );

    // 초기 대상은 없음
    TargetEnemy = nullptr;
}

void ASlimeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
    // CollisionComponent에 다른 액터가 겹치면
    // OnProjectileOverlap 함수를 호출하도록 연결
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(
        this,
        &ASlimeProjectile::OnProjectileOverlap
    );
}

void ASlimeProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 목표가 없거나 이미 죽은 Enemy라면 Projectile 제거
    if (!IsValid(TargetEnemy) || TargetEnemy->IsDead())
    {
        Destroy();
        return;
    }

    // 현재 Projectile 위치
    const FVector CurrentLocation = GetActorLocation();

    // Projectile에서 적으로 향하는 방향 계산
    FVector Direction =
        TargetEnemy->GetActorLocation() - CurrentLocation;

    // 탑다운 게임이므로 높이 차이는 무시
    Direction.Z = 0.f;

    // 방향 벡터 정규화
    Direction.Normalize();

    // 이동 거리 계산
    const FVector MoveDistance =
        Direction * MoveSpeed * DeltaTime;

    // Projectile 이동
    AddActorWorldOffset(MoveDistance, true);

    // 이동 방향을 바라보도록 회전
    SetActorRotation(Direction.Rotation());
}

void ASlimeProjectile::SetTargetEnemy(ASlimeEnemy* NewTargetEnemy)
{
    // Weapon에서 전달받은 적을 저장
    TargetEnemy = NewTargetEnemy;

    // 목표가 없거나 이미 죽은 Enemy라면 Projectile 제거
    if (!IsValid(TargetEnemy) || TargetEnemy->IsDead())
    {
        Destroy();
        return;
    }

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("투사체 대상 설정: %s"),
            *TargetEnemy->GetName()
        );
}

void ASlimeProjectile::OnProjectileOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    // 자기 자신과 겹친 경우 무시
    if (OtherActor == this)
    {
        return;
    }

    // 겹친 액터가 SlimeEnemy인지 확인
    ASlimeEnemy* HitEnemy =
        Cast<ASlimeEnemy>(OtherActor);

    // SlimeEnemy가 아니라면 무시
    if (!IsValid(HitEnemy))
    {
        return;
    }

    // 이미 죽은 Enemy라면 무시
    if (HitEnemy->IsDead())
    {
        return;
    }

    // Overlap이 정상 작동하는지 로그 출력
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Projectile 적중 | Damage: %.1f"),
        Damage
    );

    // 적에게 Weapon에서 전달받은 데미지 적용
    HitEnemy->TakeDamageFromProjectile(Damage);

    // 적에게 닿은 Projectile 제거
    Destroy();
}

void ASlimeProjectile::SetDamage(float NewDamage)
{
    // 음수 데미지가 저장되지 않도록 0 이상으로 보정
    Damage = FMath::Max(0.f, NewDamage);
}
