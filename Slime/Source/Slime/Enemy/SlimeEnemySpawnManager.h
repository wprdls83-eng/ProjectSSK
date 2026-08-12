// SlimeEnemySpawnManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slime/Data/SlimeDataTypes.h"
#include "SlimeEnemySpawnManager.generated.h"

class ASlimeEnemy;

UCLASS()
class SLIME_API ASlimeEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlimeEnemySpawnManager();

protected:
    // 플레이어 주변 생성 반경
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    float SpawnRadius = 1500.f;

    // 실제 Wave 시작 전 대기 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float WaveStartDelay = 3.f;

    // Wave Clear 이후 대기 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float WaveClearDelay = 3.f;

    // Wave별 설정 데이터를 저장하는 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TArray<FWaveData> WaveDataList;

    // 현재 진행 중인 Wave의 인덱스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWaveIndex = 0;

    // 현재 Wave에서 지금까지 생성한 Enemy 수
    int32 SpawnedEnemyCount = 0;

    // 현재 Wave에서 처치한 Enemy 수
    int32 KilledEnemyCount = 0;

    // Enemy 생성 타이머
    FTimerHandle SpawnTimerHandle;

    // Wave 종료 타이머
    FTimerHandle WaveTimerHandle;

    // Wave 전환 Timer
    FTimerHandle WaveTransitionTimerHandle;

protected:
	virtual void BeginPlay() override;


public:
    // Enemy 여러 마리 생성 처리
    void SpawnEnemy();

    // Enemy 한 마리 생성
    bool SpawnSingleEnemy();

    // 현재 Wave를 시작
    void StartWave();

    // 현재 Wave 종료
    void EndWave();

    // Enemy가 처치되었을 때 호출
    void NotifyEnemyKilled();

    // 제한 시간 전에 모든 Enemy를 처치했을 때 호출
    void ClearWaveEarly();

    // Early Clear 보상 지급
    void GiveEarlyClearReward();

    // 대기 시간이 끝난 후 다음 Wave 시작
    void StartNextWave();

    // Wave Clear 대기가 끝난 후 다음 Wave 시작 전 대기 시작
    void PrepareNextWave();
};
