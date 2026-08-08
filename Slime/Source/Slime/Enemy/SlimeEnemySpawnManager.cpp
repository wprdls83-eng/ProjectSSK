// SlimeEnemySpawnManager.cpp

#include "Slime/Enemy/SlimeEnemySpawnManager.h"
#include "Slime/Character/SlimeEnemy.h"
#include "Slime/Character/SlimeCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

ASlimeEnemySpawnManager::ASlimeEnemySpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ASlimeEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
    // Wave 시작
    StartWave();
}

void ASlimeEnemySpawnManager::SpawnEnemy()
{       
    // 현재 Wave 데이터가 유효하지 않다면 종료
    if (!WaveDataList.IsValidIndex(CurrentWaveIndex))
    {
        return;
    }

    // 현재 Wave 데이터 가져오기
    const FWaveData& CurrentWaveData =
        WaveDataList[CurrentWaveIndex];

    // 한 번에 생성할 Enemy 수만큼 반복
    for (int32 i = 0; i < CurrentWaveData.EnemiesPerSpawn; ++i)
    {
        // 이번 Wave에서 생성할 총 Enemy 수를 모두 채웠다면 종료
        if (SpawnedEnemyCount >= CurrentWaveData.TotalEnemies)
        {
            break;
        }

        // 현재 월드에 존재하는 Enemy 검색
        TArray<AActor*> FoundEnemies;

        UGameplayStatics::GetAllActorsOfClass(
            this,
            ASlimeEnemy::StaticClass(),
            FoundEnemies
        );

        // 최대 생존 Enemy 수에 도달했다면 생성 중지
        if (FoundEnemies.Num() >= CurrentWaveData.MaxAliveEnemies)
        {
            break;
        }

        // Enemy 생성에 성공했을 때만 생성 수 증가
        if (SpawnSingleEnemy())
        {
            SpawnedEnemyCount++;
        }
    }
}

bool ASlimeEnemySpawnManager::SpawnSingleEnemy()
{
    // 현재 Wave 데이터가 유효하지 않다면 실패
    if (!WaveDataList.IsValidIndex(CurrentWaveIndex))
    {
        return false;
    }

    // 현재 Wave 데이터 가져오기
    const FWaveData& CurrentWaveData =
        WaveDataList[CurrentWaveIndex];

    // 등록된 Enemy Spawn 데이터가 없다면 실패
    if (CurrentWaveData.EnemySpawnDataList.IsEmpty())
    {
        return false;
    }

    // 전체 Spawn 가중치 계산
    int32 TotalWeight = 0;

    for (const FEnemySpawnData& SpawnData : CurrentWaveData.EnemySpawnDataList)
    {
        TotalWeight += SpawnData.SpawnWeight;
    }

    // 정상적인 가중치가 없다면 실패
    if (TotalWeight <= 0)
    {
        return false;
    }

    // 1 ~ 전체 가중치 사이에서 랜덤 값 선택
    const int32 RandomWeight =
        FMath::RandRange(1, TotalWeight);

    // 랜덤으로 선택될 Enemy 클래스
    TSubclassOf<ASlimeEnemy> SelectedEnemyClass = nullptr;

    // 누적 가중치
    int32 AccumulatedWeight = 0;

    for (const FEnemySpawnData& SpawnData : CurrentWaveData.EnemySpawnDataList)
    {
        AccumulatedWeight += SpawnData.SpawnWeight;

        // 랜덤 값이 현재 누적 범위에 들어오면 이 Enemy 선택
        if (RandomWeight <= AccumulatedWeight)
        {
            SelectedEnemyClass = SpawnData.EnemyClass;
            break;
        }
    }

    // 선택된 Enemy 클래스가 없다면 실패
    if (!SelectedEnemyClass)
    {
        return false;
    }

    // 선택된 클래스가 유효하지 않다면 실패
    if (!SelectedEnemyClass)
    {
        return false;
    }

    // 플레이어 캐릭터 가져오기
    ACharacter* PlayerCharacter =
        UGameplayStatics::GetPlayerCharacter(this, 0);

    // 플레이어가 유효하지 않다면 종료
    if (!IsValid(PlayerCharacter))
    {
        return false;
    }

    // 플레이어 위치 가져오기
    const FVector PlayerLocation =
        PlayerCharacter->GetActorLocation();

    // 0도부터 360도 사이의 랜덤 각도
    const float RandomAngle =
        FMath::RandRange(0.f, 360.f);

    // 도 단위를 라디안으로 변환
    const float Radian =
        FMath::DegreesToRadians(RandomAngle);

    // 플레이어 주변의 랜덤 방향
    const FVector Direction(
        FMath::Cos(Radian),
        FMath::Sin(Radian),
        0.f
    );

    // 최종 생성 위치
    const FVector SpawnLocation =
        PlayerLocation +
        Direction * SpawnRadius;

    // 선택된 Enemy 생성
    ASlimeEnemy* SpawnedEnemy =
        GetWorld()->SpawnActor<ASlimeEnemy>(
            SelectedEnemyClass,
            SpawnLocation,
            FRotator::ZeroRotator
        );

    // 실제 생성 성공 여부 반환
    return IsValid(SpawnedEnemy);
}

void ASlimeEnemySpawnManager::StartWave()
{
    // 현재 Wave의 생성 수 초기화
    SpawnedEnemyCount = 0;

    // 현재 Wave의 처치 수 초기화
    KilledEnemyCount = 0;

    // 사용할 수 있는 Wave 데이터가 없다면 종료
    if (!WaveDataList.IsValidIndex(CurrentWaveIndex))
    {
        return;
    }

    GEngine->AddOnScreenDebugMessage(
        -1,
        2.f,
        FColor::Green,
        FString::Printf(
            TEXT("Wave %d Start"),
            CurrentWaveIndex + 1
        )
    );

    // 현재 Wave 데이터 가져오기
    const FWaveData& CurrentWaveData =
        WaveDataList[CurrentWaveIndex];

    // 기존 Spawn Timer가 있다면 제거
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);


    // 현재 Wave의 Spawn Timer 시작
    GetWorldTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ASlimeEnemySpawnManager::SpawnEnemy,
        CurrentWaveData.SpawnInterval,
        true
    );

    // 현재 Wave 시간이 끝나면 EndWave 호출
    GetWorldTimerManager().SetTimer(
        WaveTimerHandle,
        this,
        &ASlimeEnemySpawnManager::EndWave,
        CurrentWaveData.WaveDuration,
        false
    );
}

void ASlimeEnemySpawnManager::EndWave()
{
    // Enemy Spawn 중지
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

    // 현재 월드에 남아 있는 Enemy 검색
    TArray<AActor*> FoundEnemies;

    UGameplayStatics::GetAllActorsOfClass(
        this,
        ASlimeEnemy::StaticClass(),
        FoundEnemies
    );

    // 시간 초과로 남아 있는 Enemy 전부 제거
    for (AActor* Enemy : FoundEnemies)
    {
        if (IsValid(Enemy))
        {
            Enemy->Destroy();
        }
    }

    // 다음 Wave로 이동
    CurrentWaveIndex++;

    // 다음 Wave가 존재하지 않는다면 종료
    if (!WaveDataList.IsValidIndex(CurrentWaveIndex))
    {
        return;
    }

    // 다음 Wave 즉시 시작
    StartWave();
}

void ASlimeEnemySpawnManager::NotifyEnemyKilled()
{
    // 처치 수 증가
    KilledEnemyCount++;

    // 현재 Wave 데이터가 유효하지 않다면 종료
    if (!WaveDataList.IsValidIndex(CurrentWaveIndex))
    {
        return;
    }

    const FWaveData& CurrentWaveData =
        WaveDataList[CurrentWaveIndex];

    // 제한 시간 전에 모든 Enemy를 처치했다면 조기 클리어
    if (KilledEnemyCount >= CurrentWaveData.TotalEnemies)
    {
        ClearWaveEarly();
    }
}

void ASlimeEnemySpawnManager::ClearWaveEarly()
{   
    // 조기 클리어 확인용 메시지
    GEngine->AddOnScreenDebugMessage(
        -1,
        3.f,
        FColor::Green,
        FString::Printf(
            TEXT("Wave %d 조기 클리어!"),
            CurrentWaveIndex + 1
        )
    );

    // Spawn Timer 중지
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

    // Wave 종료 Timer 중지
    GetWorldTimerManager().ClearTimer(WaveTimerHandle);

    // Early Clear 보상 지급
    GiveEarlyClearReward();

    // 다음 Wave로 이동
    CurrentWaveIndex++;

    // 다음 Wave가 존재하지 않는다면 종료
    if (!WaveDataList.IsValidIndex(CurrentWaveIndex))
    {
        return;
    }

    // 다음 Wave 즉시 시작
    StartWave();
}

void ASlimeEnemySpawnManager::GiveEarlyClearReward()
{
    // 0 ~ 5 사이에서 랜덤 보상 선택
    const int32 RandomRewardIndex =
        FMath::RandRange(0, 5);

    const EEarlyClearRewardType RewardType =
        static_cast<EEarlyClearRewardType>(RandomRewardIndex);

    // 현재 플레이어 가져오기
    ASlimeCharacter* PlayerCharacter =
        Cast<ASlimeCharacter>(
            UGameplayStatics::GetPlayerCharacter(this, 0)
        );

    // 플레이어가 유효하지 않다면 종료
    if (!IsValid(PlayerCharacter))
    {
        return;
    }

    // 선택된 보상을 실제 플레이어에게 적용
    PlayerCharacter->ApplyEarlyClearReward(RewardType);

    // 화면에 표시할 보상 이름
    FString RewardName;

    switch (RewardType)
    {
    case EEarlyClearRewardType::Damage:
        RewardName = TEXT("Damage UP");
        break;

    case EEarlyClearRewardType::AttackSpeed:
        RewardName = TEXT("Attack Speed UP");
        break;

    case EEarlyClearRewardType::AttackRange:
        RewardName = TEXT("Attack Range UP");
        break;

    case EEarlyClearRewardType::MaxHealth:
        RewardName = TEXT("Max Health UP");
        break;

    case EEarlyClearRewardType::MoveSpeed:
        RewardName = TEXT("Move Speed UP");
        break;

    case EEarlyClearRewardType::Exp:
        RewardName = TEXT("EXP +50");
        break;
    }

    // 테스트용 화면 출력
    GEngine->AddOnScreenDebugMessage(
        -1,
        3.f,
        FColor::Yellow,
        FString::Printf(
            TEXT("Early Clear Reward : %s"),
            *RewardName
        )
    );
}