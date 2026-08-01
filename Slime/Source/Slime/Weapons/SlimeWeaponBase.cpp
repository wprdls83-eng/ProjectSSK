// SlimeWeaponBase.cpp

#include "Slime/Weapons/SlimeWeaponBase.h"
#include "Slime/Character/SlimeEnemy.h"
#include "Slime/Weapons/SlimeProjectile.h"

#include "EngineUtils.h"

// Sets default values
ASlimeWeaponBase::ASlimeWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AttackInterval = 1.5f;
	AttackRange = 2000.f;
    Damage = 10.f;
}

void ASlimeWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작되면 자동 공격 타이머 시작
	StartAttackTimer();
}

void ASlimeWeaponBase::StartAttackTimer()
{
	// 공격 간격이 0 이하라면 타이머를 시작하지 않음
	if (AttackInterval <= 0.f)
	{
		return;
	}

	// 설정된 시간마다 AutoAttack 함수를 반복 실행
	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ASlimeWeaponBase::AutoAttack,
		AttackInterval,
		true
	);
}

void ASlimeWeaponBase::AutoAttack()
{
    // 공격 범위 안에서 가장 가까운 적 찾기
    ASlimeEnemy* TargetEnemy = FindClosestEnemy();

    // 공격할 적이 없다면 종료
    if (!IsValid(TargetEnemy))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("공격 범위 안에 적이 없습니다.")
        );

        return;
    }

    // 선택된 적의 이름을 로그로 출력
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("가장 가까운 적: %s"),
        *TargetEnemy->GetName()
    );

    // 선택된 적을 향해 투사체 생성
    SpawnProjectile(TargetEnemy);
}

ASlimeEnemy* ASlimeWeaponBase::FindClosestEnemy() const
{
    // 현재까지 발견한 가장 가까운 적을 저장
    ASlimeEnemy* ClosestEnemy = nullptr;

    // 공격 범위의 제곱을 초기 기준 거리로 사용
    float ClosestDistanceSquared = AttackRange * AttackRange;

    // 현재 무기의 위치
    const FVector WeaponLocation = GetActorLocation();

    // 월드에 존재하는 모든 SlimeEnemy를 하나씩 확인
    for (TActorIterator<ASlimeEnemy> It(GetWorld()); It; ++It)
    {
        // 현재 반복문에서 확인 중인 적
        ASlimeEnemy* Enemy = *It;

        // 적이 유효하지 않으면 다음 적으로 넘어감
        if (!IsValid(Enemy))
        {
            continue;
        }

        // 무기와 적 사이의 거리 제곱을 계산
        const float DistanceSquared = FVector::DistSquared(
            WeaponLocation,
            Enemy->GetActorLocation()
        );

        // 지금까지 찾은 적보다 더 가까운 경우
        if (DistanceSquared < ClosestDistanceSquared)
        {
            // 가장 가까운 거리 갱신
            ClosestDistanceSquared = DistanceSquared;

            // 가장 가까운 적 갱신
            ClosestEnemy = Enemy;
        }
    }

    // 가장 가까운 적 반환
    return ClosestEnemy;
}

void ASlimeWeaponBase::SpawnProjectile(ASlimeEnemy* TargetEnemy)
{
    // 대상이 유효하지 않으면 생성하지 않음
    if (!IsValid(TargetEnemy))
    {
        return;
    }

    // 투사체 클래스가 설정되지 않았다면 생성할 수 없음
    if (!ProjectileClass)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("ProjectileClass가 설정되지 않았습니다.")
        );

        return;
    }

    // 투사체 생성 위치
    const FVector SpawnLocation = GetActorLocation();

    // 무기에서 적을 향하는 방향 계산
    const FVector TargetDirection =
        TargetEnemy->GetActorLocation() - SpawnLocation;

    // 방향 벡터를 회전값으로 변환
    const FRotator SpawnRotation =
        TargetDirection.Rotation();

    // 투사체 생성
    ASlimeProjectile* SpawnedProjectile =
        GetWorld()->SpawnActor<ASlimeProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation
        );

    // 생성에 성공했다면 공격 대상을 전달
    if (IsValid(SpawnedProjectile))
    {
        // 공격할 대상 전달
        SpawnedProjectile->SetTargetEnemy(TargetEnemy);

        // 현재 무기의 데미지 전달
        SpawnedProjectile->SetDamage(Damage);

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Projectile 생성 성공")
        );
    }
}


