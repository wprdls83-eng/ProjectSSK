// SlimeMage.cpp

#include "Slime/Player/SlimeMage.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

ASlimeMage::ASlimeMage()
{

}

void ASlimeMage::BeginPlay()
{
	// 마법사 전용 최대 체력 적용
	MaxHealth = MageMaxHealth;

	// 마법사 전용 이동 속도 적용
	GetCharacterMovement()->MaxWalkSpeed =
		MageMoveSpeed;

	// 부모 BeginPlay 실행
	Super::BeginPlay();
}

void ASlimeMage::UseSpecialAbility()
{
	// 쿨타임 중이라면 사용 불가
	if (!bCanReflect)
	{
		return;
	}

	// 사망한 상태라면 사용 불가
	if (bIsDead)
	{
		return;
	}

	// 반사 상태 시작
	bIsReflecting = true;

	// 재사용 불가
	bCanReflect = false;

	// 일정 시간 후 반사 종료
	GetWorldTimerManager().SetTimer(
		ReflectDurationTimerHandle,
		this,
		&ASlimeMage::EndReflect,
		ReflectDuration,
		false
	);

	// 발동 순간부터 쿨타임 시작
	GetWorldTimerManager().SetTimer(
		ReflectCooldownTimerHandle,
		this,
		&ASlimeMage::ResetReflectCooldown,
		ReflectCooldown,
		false
	);

	// 테스트 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Purple,
			TEXT("REFLECT ACTIVE!")
		);
	}
}

void ASlimeMage::EndReflect()
{
	// 반사 상태 종료
	bIsReflecting = false;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::White,
			TEXT("Reflect End")
		);
	}
}

void ASlimeMage::ResetReflectCooldown()
{
	// 다시 반사 능력 사용 가능
	bCanReflect = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			TEXT("Reflect Ready")
		);
	}
}

bool ASlimeMage::IsReflecting() const
{
	return bIsReflecting;
}

TArray<EPlayerUpgradeType> ASlimeMage::GetAvailableUpgrades() const
{
	TArray<EPlayerUpgradeType> Upgrades =
		Super::GetAvailableUpgrades();

	// 마법사 전용 업그레이드
	Upgrades.Add(
		EPlayerUpgradeType::MageExplosionRadius
	);

	Upgrades.Add(
		EPlayerUpgradeType::MageReflectDuration
	);

	Upgrades.Add(
		EPlayerUpgradeType::MageReflectDamage
	);

	return Upgrades;
}

void ASlimeMage::UpgradeReflectDuration()
{
	ReflectDuration +=
		ReflectDurationUpgradeAmount;
}

void ASlimeMage::UpgradeReflectDamage()
{
	ReflectDamageMultiplier +=
		ReflectDamageMultiplierUpgradeAmount;
}