// SlimeWeaponBase.cpp

#include "Slime/Weapons/SlimeWeaponBase.h"
#include "Slime/Character/SlimeEnemy.h"
#include "Slime/Weapons/SlimeProjectile.h"

#include "EngineUtils.h"

ASlimeWeaponBase::ASlimeWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackInterval = 1.f;
	AttackRange = 500.f;
    Damage = 30.f;
    ProjectileCount = 1;
}

void ASlimeWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작되면 자동 공격 타이머 시작
	StartAttackTimer();
}

void ASlimeWeaponBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 현재 무기의 공격 범위를 원으로 표시
    DrawDebugCircle(
        GetWorld(),
        GetActorLocation(),
        AttackRange,
        64,
        FColor::Green,
        false,
        -1.f,
        0,
        2.f,
        FVector(1.f, 0.f, 0.f),
        FVector(0.f, 1.f, 0.f),
        false
    );
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
        return;
    }

    // 투사체 사이 간격
    const float ProjectileSpacing = 100.f;

    // 전체 투사체가 가운데를 기준으로 정렬되도록 시작 위치 계산
    const float StartOffset =
        -((ProjectileCount - 1) * ProjectileSpacing) / 2.f;

    for (int32 i = 0; i < ProjectileCount; ++i)
    {
        // 현재 투사체의 좌우 위치 보정값
        const float Offset =
            StartOffset + (i * ProjectileSpacing);

        SpawnProjectile(TargetEnemy, Offset);
    }
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

        // 죽은 Enemy는 타겟에서 제외
        if (Enemy->IsDead())
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

void ASlimeWeaponBase::SpawnProjectile(ASlimeEnemy* TargetEnemy, float SideOffset)
{
    // 대상이 유효하지 않으면 생성하지 않음
    if (!IsValid(TargetEnemy))
    {
        return;
    }

    // 이미 죽은 Enemy라면 투사체 생성하지 않음
    if (TargetEnemy->IsDead())
    {
        return;
    }

    // 투사체 클래스가 설정되지 않았다면 생성할 수 없음
    if (!ProjectileClass)
    {
        return;
    }

    // 기본 생성 위치
    FVector SpawnLocation = GetActorLocation();

    // 무기의 오른쪽 방향으로 위치를 보정
    SpawnLocation += GetActorRightVector() * SideOffset;

    // 적을 향하는 방향 계산
    const FVector TargetDirection =
        TargetEnemy->GetActorLocation() - SpawnLocation;

    // 방향을 회전값으로 변환
    const FRotator SpawnRotation =
        TargetDirection.Rotation();

    // 투사체 생성
    ASlimeProjectile* SpawnedProjectile =
        GetWorld()->SpawnActor<ASlimeProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation
        );

    // 생성된 투사체에 대상과 데미지 전달
    if (IsValid(SpawnedProjectile))
    {
        SpawnedProjectile->SetTargetEnemy(TargetEnemy);
        SpawnedProjectile->SetDamage(Damage);
    }
}

void ASlimeWeaponBase::StopAttackTimer()
{
    // 자동 공격 Timer 중지
    GetWorldTimerManager().ClearTimer(
        AttackTimerHandle
    );
}

void ASlimeWeaponBase::UpgradeDamage()
{   
    // 설정된 증가량만큼 공격력 증가
    Damage += DamageUpgradeAmount;
}

void ASlimeWeaponBase::UpgradeAttackSpeed()
{   
    // 공격 간격 감소
    AttackInterval -= AttackIntervalUpgradeAmount;

    // 너무 빨라지는 것을 방지
    AttackInterval = FMath::Max(
        0.1f,
        AttackInterval
    );

    // 기존 공격 타이머를 새로운 공격 속도로 다시 시작
    StopAttackTimer();
    StartAttackTimer();
}

void ASlimeWeaponBase::UpgradeProjectileCount()
{
    // 발사체 개수 1 증가
    ProjectileCount += ProjectileCountUpgradeAmount;
}

void ASlimeWeaponBase::UpgradeAttackRange()
{
    // 설정된 증가량만큼 공격 범위 증가
    AttackRange += AttackRangeUpgradeAmount;
}

