// SlimeProjectile.cpp

#include "Slime/Weapons/SlimeProjectile.h"
#include "Slime/Character/SlimeEnemy.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ASlimeProjectile::ASlimeProjectile()
{

	PrimaryActorTick.bCanEverTick = false;

    // 충돌 컴포넌트 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(
        TEXT("CollisionComponent")
    );

    // 충돌 컴포넌트를 루트로 지정
    SetRootComponent(CollisionComponent);

    // 충돌 구체 크기 설정
    CollisionComponent->SetSphereRadius(20.f);

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
	
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("투사체가 생성되었습니다.")
    );
}

void ASlimeProjectile::SetTargetEnemy(ASlimeEnemy* NewTargetEnemy)
{
    // Weapon에서 전달받은 적을 저장
    TargetEnemy = NewTargetEnemy;

    if (IsValid(TargetEnemy))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("투사체 대상 설정: %s"),
            *TargetEnemy->GetName()
        );
    }
}

