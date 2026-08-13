// SlimeExpOrbBase.cpp

#include "Slime/Item/SlimeExpOrbBase.h"
#include "Slime/Character/SlimeCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ASlimeExpOrbBase::ASlimeExpOrbBase()
{
	PrimaryActorTick.bCanEverTick = false;

    // 플레이어와 겹침을 감지할 Sphere Collision 생성
    CollisionComponent =
        CreateDefaultSubobject<USphereComponent>(
            TEXT("CollisionComponent")
        );

    // Collision을 루트 컴포넌트로 설정
    SetRootComponent(CollisionComponent);

    // 경험치 오브 획득 범위
    CollisionComponent->SetSphereRadius(40.f);

    // 충돌 감지만 사용
    CollisionComponent->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );

    // 모든 채널은 우선 무시
    CollisionComponent->SetCollisionResponseToAllChannels(
        ECR_Ignore
    );

    // 플레이어가 사용하는 Pawn 채널과만 Overlap
    CollisionComponent->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Overlap
    );

    // Overlap 이벤트 활성화
    CollisionComponent->SetGenerateOverlapEvents(true);

    // 경험치 오브 외형을 표시할 Mesh 생성
    MeshComponent =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("MeshComponent")
        );

    // Mesh를 Collision에 부착
    MeshComponent->SetupAttachment(CollisionComponent);

    // Mesh 자체의 충돌은 비활성화
    MeshComponent->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );
}

void ASlimeExpOrbBase::BeginPlay()
{
	Super::BeginPlay();
	
    // 플레이어와 겹치면 OnExpOrbOverlap 함수 호출
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(
        this,
        &ASlimeExpOrbBase::OnExpOrbOverlap
    );
}

void ASlimeExpOrbBase::OnExpOrbOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    // 자기 자신과 겹친 경우 무시
    if (OtherActor == this)
    {
        return;
    }

    // 플레이어 캐릭터인지 확인
    ASlimeCharacter* PlayerCharacter =
        Cast<ASlimeCharacter>(OtherActor);

    // 플레이어가 아니라면 무시
    if (!IsValid(PlayerCharacter))
    {
        return;
    }

    PlayerCharacter->AddExp(ExpAmount);

    // 획득한 경험치 오브 제거
    Destroy();
}

void ASlimeExpOrbBase::SetExpAmount(int32 NewExpAmount)
{
    // 잘못된 경험치 값 방지
    ExpAmount = FMath::Max(0, NewExpAmount);
}