// SlimeArcher.cpp

#include "Slime/Player/SlimeArcher.h"

#include "GameFramework/CharacterMovementComponent.h"

ASlimeArcher::ASlimeArcher()
{

}

void ASlimeArcher::BeginPlay()
{
	// 궁수 전용 최대 체력 적용
	MaxHealth = ArcherMaxHealth;

	// 궁수 전용 이동 속도 적용
	GetCharacterMovement()->MaxWalkSpeed =
		ArcherMoveSpeed;

	// 부모 BeginPlay 실행
	Super::BeginPlay();
}

void ASlimeArcher::UseSpecialAbility()
{
	// 쿨타임 중이라면 대쉬 불가능
	if (!bCanDash)
	{
		return;
	}

	// 사망 상태라면 대쉬 불가능
	if (bIsDead)
	{
		return;
	}

	// 대쉬할 방향
	FVector DashDirection =
		CurrentMoveDirection;

	// 이동 입력이 없다면
	if (DashDirection.IsNearlyZero())
	{
		// 현재 캐릭터가 바라보는 방향으로 대쉬
		DashDirection =
			GetActorForwardVector();
	}

	// 높이 방향 제거
	DashDirection.Z = 0.f;

	// 방향 정규화
	DashDirection.Normalize();

	// 해당 방향으로 캐릭터 발사
	LaunchCharacter(
		DashDirection * DashSpeed,
		true,
		false
	);

	// 대쉬 재사용 불가
	bCanDash = false;

	// 쿨타임 시작
	GetWorldTimerManager().SetTimer(
		DashCooldownTimerHandle,
		this,
		&ASlimeArcher::ResetDashCooldown,
		DashCooldown,
		false
	);
}

void ASlimeArcher::ResetDashCooldown()
{
	// 다시 대쉬 사용 가능
	bCanDash = true;
}

TArray<EPlayerUpgradeType> ASlimeArcher::GetAvailableUpgrades() const
{
	TArray<EPlayerUpgradeType> Upgrades =
		Super::GetAvailableUpgrades();

	// 궁수 전용 업그레이드
	Upgrades.Add(
		EPlayerUpgradeType::ArcherProjectileCount
	);

	Upgrades.Add(
		EPlayerUpgradeType::ArcherDashCooldown
	);

	return Upgrades;
}

void ASlimeArcher::UpgradeDashCooldown()
{
	DashCooldown -= DashCooldownUpgradeAmount;

	// 너무 짧아지지 않도록 최소 쿨타임 제한
	DashCooldown = FMath::Max(
		1.f,
		DashCooldown
	);
}