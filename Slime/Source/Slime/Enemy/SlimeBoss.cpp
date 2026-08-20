// SlimeBoss.cpp

#include "Slime/Enemy/SlimeBoss.h"
#include "Slime/Enemy/SlimeEnemySpawnManager.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASlimeBoss::ASlimeBoss()
{
	// Boss 기본 체력
	MaxHealth = 500.f;

	// Boss 접촉 데미지
	AttackDamage = 30.f;

	// Boss 접촉 데미지 재적용 시간
	ContactDamageCooldown = 1.5f;

	// Boss 이동속도
	GetCharacterMovement()->MaxWalkSpeed = 180.f;
}

void ASlimeBoss::BeginPlay()
{
	Super::BeginPlay();
}

void ASlimeBoss::Die()
{
    // 현재 월드의 Enemy Spawn Manager 가져오기
    ASlimeEnemySpawnManager* SpawnManager =
        Cast<ASlimeEnemySpawnManager>(
            UGameplayStatics::GetActorOfClass(
                this,
                ASlimeEnemySpawnManager::StaticClass()
            )
        );

    // Spawn Manager에게 Boss가 처치됐다고 알림
    if (IsValid(SpawnManager))
    {
        SpawnManager->NotifyBossKilled();
    }

    // Boss 제거
    Destroy();
}

float ASlimeBoss::GetBossCurrentHealth() const
{
	return CurrentHealth;
}

float ASlimeBoss::GetBossMaxHealth() const
{
	return MaxHealth;
}

float ASlimeBoss::GetBossHealthPercent() const
{
	// 0으로 나누는 것을 방지
	if (MaxHealth <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(
		CurrentHealth / MaxHealth,
		0.f,
		1.f
	);
}
