// SlimeWarrior.cpp

#include "Slime/Player/SlimeWarrior.h"
#include "Slime/Character/SlimeEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"

ASlimeWarrior::ASlimeWarrior()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASlimeWarrior::BeginPlay()
{
	// 전사 전용 능력치를 먼저 적용
	MaxHealth = WarriorMaxHealth;

	GetCharacterMovement()->MaxWalkSpeed =
		WarriorMoveSpeed;

	// 부모 BeginPlay 실행
	Super::BeginPlay();
}

void ASlimeWarrior::UseSpecialAbility()
{
	// 쿨타임 중이라면 사용할 수 없음
	if (!bCanUseSpecialAbility)
	{
		return;
	}

	// 사망한 상태라면 사용할 수 없음
	if (bIsDead)
	{
		return;
	}

	// 무적 상태 시작
	bIsInvincible = true;

	// 고유 능력 재사용 불가
	bCanUseSpecialAbility = false;

	// 일정 시간이 지나면 무적 종료
	GetWorldTimerManager().SetTimer(
		InvincibleTimerHandle,
		this,
		&ASlimeWarrior::EndInvincibility,
		InvincibleDuration,
		false
	);

	// 쿨타임 시작
	GetWorldTimerManager().SetTimer(
		SpecialAbilityCooldownTimerHandle,
		this,
		&ASlimeWarrior::ResetSpecialAbilityCooldown,
		SpecialAbilityCooldown,
		false
	);

	// 테스트 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Yellow,
			TEXT("INVINCIBLE!")
		);
	}
}

bool ASlimeWarrior::CanTakeDamageFromEnemy() const
{
	// 무적 중이라면 데미지를 받을 수 없음
	if (bIsInvincible)
	{
		return false;
	}

	// 무적이 아니라면 부모의 기본 판정 사용
	return Super::CanTakeDamageFromEnemy();
}

void ASlimeWarrior::EndInvincibility()
{
	// 무적 상태 종료
	bIsInvincible = false;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::White,
			TEXT("Invincibility End")
		);
	}
}

void ASlimeWarrior::ResetSpecialAbilityCooldown()
{
	// 다시 고유 능력 사용 가능
	bCanUseSpecialAbility = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			TEXT("Special Ability Ready")
		);
	}
}

TArray<EPlayerUpgradeType> ASlimeWarrior::GetAvailableUpgrades() const
{
	// 부모의 공통 업그레이드 목록 가져오기
	TArray<EPlayerUpgradeType> Upgrades =
		Super::GetAvailableUpgrades();

	// 전사 전용 업그레이드 추가
	Upgrades.Add(
		EPlayerUpgradeType::WarriorMaxHitEnemies
	);

	Upgrades.Add(
		EPlayerUpgradeType::WarriorAttackAngle
	);

	return Upgrades;
}