// SlimeEnemySpawnManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Slime/Data/SlimeDataTypes.h"
#include "SlimeEnemySpawnManager.generated.h"

class ASlimeEnemy;
class UUserWidget;

UCLASS()
class SLIME_API ASlimeEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlimeEnemySpawnManager();

    // 현재 Wave의 남은 시간 반환
    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    float GetRemainingWaveTime() const;

    // 현재 Wave에서 남아 있는 Enemy 수 반환
    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    int32 GetRemainingEnemyCount() const;

    // 현재 Wave의 전체 Enemy 수 반환
    UFUNCTION(BlueprintPure, Category = "Wave|UI")
    int32 GetTotalEnemyCount() const;

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave|UI")
    bool bIsWaveActive = false;

    // 마지막 Wave가 조기 클리어에 성공했는지 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave|UI")
    bool bLastEarlyClearSuccess = false;

    // Wave별 설정 데이터를 저장하는 배열
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TArray<FWaveData> WaveDataList;

    // Wave 시작 카운트다운 UI 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|UI")
    TSubclassOf<UUserWidget> WaveCountdownWidgetClass;

    // 생성된 카운트다운 UI
    UPROPERTY()
    TObjectPtr<UUserWidget> WaveCountdownWidget;

    // 현재 진행 중인 Wave의 인덱스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CurrentWaveIndex = 0;

    // 현재 Wave에서 지금까지 생성한 Enemy 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave|UI")
    int32 SpawnedEnemyCount = 0;

    // 현재 Wave에서 처치한 Enemy 수
    int32 KilledEnemyCount = 0;

    // 현재 카운트다운 숫자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 CountdownValue = 0;

    // 마지막 조기 클리어 보상 이름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave|UI")
    FString LastEarlyClearRewardText;

    // 카운트다운 UI 갱신 Timer
    FTimerHandle CountdownTimerHandle;

    // Enemy 생성 타이머
    FTimerHandle SpawnTimerHandle;

    // Wave 종료 타이머
    FTimerHandle WaveTimerHandle;

    // Wave 전환 Timer
    FTimerHandle WaveTransitionTimerHandle;

protected:
	virtual void BeginPlay() override;

    // Wave 시작 카운트다운 시작
    void StartWaveCountdown();

    // 1초마다 카운트다운 숫자 감소
    void UpdateWaveCountdown();

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

    // Wave 결과 UI 표시 요청
    UFUNCTION(BlueprintImplementableEvent, Category = "Wave|UI")
    void ShowWaveResultUI(
        bool bSuccess,
        const FString& RewardText
    );
};
