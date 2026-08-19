// SlimeDataTypes.H

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

// 레벨업 시 선택할 수 있는 업그레이드 종류 Enum
UENUM(BlueprintType)
enum class EPlayerUpgradeType : uint8
{
	// 공통 업그레이드
	Damage,
	AttackSpeed,
	AttackRange,
	MaxHealth,
	MoveSpeed,

	// 전사 전용
	WarriorMaxHitEnemies,
	WarriorAttackAngle,

	// 궁수 전용
	ArcherProjectileCount,
	ArcherDashCooldown,

	// 마법사 전용
	MageExplosionRadius,
	MageReflectDuration,
	MageReflectDamage
};

// 플레이어 직업 Enum
UENUM(BlueprintType)
enum class EPlayerJob : uint8
{
	Warrior,
	Archer,
	Mage
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

	// Wave에 따른 Enemy 체력 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyHealthMultiplier = 1.0f;

	// Wave에 따른 Enemy 공격력 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyDamageMultiplier = 1.0f;

	// Wave에 따른 Enemy 이동속도 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemyMoveSpeedMultiplier = 1.0f;

	// 현재 Wave에서 등장할 Enemy와 Spawn 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemySpawnData> EnemySpawnDataList;
};

