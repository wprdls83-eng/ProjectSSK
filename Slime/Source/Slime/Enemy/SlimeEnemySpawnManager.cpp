// SlimeEnemySpawnManager.cpp

#include "Slime/Enemy/SlimeEnemySpawnManager.h"
#include "Slime/Character/SlimeEnemy.h"

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
	
    // SpawnInterval마다 SpawnEnemy() 실행
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &ASlimeEnemySpawnManager::SpawnEnemy,
        SpawnInterval,
        true
    );
}

void ASlimeEnemySpawnManager::SpawnEnemy()
{       
    // 한 번에 생성할 Enemy 수만큼 반복
    for (int32 i = 0; i < EnemiesPerSpawn; ++i)
    {
        // 현재 월드에 존재하는 Enemy 검색
        TArray<AActor*> FoundEnemies;

        UGameplayStatics::GetAllActorsOfClass(
            this,
            ASlimeEnemy::StaticClass(),
            FoundEnemies
        );

        // 최대 생존 수에 도달했다면 반복 종료
        if (FoundEnemies.Num() >= MaxAliveEnemies)
        {
            break;
        }

        // Enemy 한 마리 생성
        SpawnSingleEnemy();
    }
}

void ASlimeEnemySpawnManager::SpawnSingleEnemy()
{
    // 생성할 Enemy 클래스가 설정되지 않았다면 종료
    if (!EnemyClass)
    {
        return;
    }

    // 플레이어 캐릭터 가져오기
    ACharacter* PlayerCharacter =
        UGameplayStatics::GetPlayerCharacter(this, 0);

    // 플레이어가 유효하지 않다면 종료
    if (!IsValid(PlayerCharacter))
    {
        return;
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

    // Enemy 생성
    GetWorld()->SpawnActor<ASlimeEnemy>(
        EnemyClass,
        SpawnLocation,
        FRotator::ZeroRotator
    );
}
