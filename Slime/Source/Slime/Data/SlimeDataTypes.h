#pragma once

#include "CoreMinimal.h"
#include "SlimeDataTypes.generated.h"

class ASlimeEnemy;

// 보상 Enum
UENUM(BlueprintType)
enum class EEarlyClearRewardType : uint8
{
	Damage,
	AttackSpeed,
	AttackRange,
	MaxHealth,
	MoveSpeed,
	Exp
};

USTRUCT(BlueprintType)
struct FEnemySpawnData
{
	GENERATED_BODY()

	// 생성할 Enemy 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASlimeEnemy> EnemyClass;

	// Spawn 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpawnWeight = 1;
};

// 하나의 Wave에 필요한 데이터를 묶어서 관리하는 구조체
USTRUCT(BlueprintType)
struct FWaveData
{
	GENERATED_BODY()

	// Wave 제한 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaveDuration = 30.f;

	// Enemy 생성 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 2.f;

	// 한 번에 생성되는 Enemy 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 EnemiesPerSpawn = 1;

	// 이번 Wave에서 총 생성할 Enemy 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalEnemies = 10;

	// 동시에 존재할 수 있는 최대 Enemy 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAliveEnemies = 10;

	// 현재 Wave에서 등장할 Enemy와 Spawn 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpawnData> EnemySpawnDataList;
};

