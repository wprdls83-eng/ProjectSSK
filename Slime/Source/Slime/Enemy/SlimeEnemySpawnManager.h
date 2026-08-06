// SlimeEnemySpawnManager.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeEnemySpawnManager.generated.h"

class ASlimeEnemy;

UCLASS()
class SLIME_API ASlimeEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlimeEnemySpawnManager();

protected:
    // 생성할 Enemy 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    TSubclassOf<ASlimeEnemy> EnemyClass;

    // Enemy 생성 간격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    float SpawnInterval = 2.f;

    // 한 번에 생성할 Enemy 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    int32 EnemiesPerSpawn = 2;

    // 최대 생존 Enemy 수
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    int32 MaxAliveEnemies = 20;

    // 플레이어 주변 생성 반경
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
    float SpawnRadius = 1500.f;

    // Enemy 생성 타이머
    FTimerHandle SpawnTimerHandle;

protected:
	virtual void BeginPlay() override;

    // Enemy 여러 마리 생성 처리
    void SpawnEnemy();

    // Enemy 한 마리 생성
    void SpawnSingleEnemy();
};
