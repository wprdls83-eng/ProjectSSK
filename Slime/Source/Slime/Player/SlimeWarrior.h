// SlimeWarrior.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeCharacter.h"
#include "SlimeWarrior.generated.h"

UCLASS()
class SLIME_API ASlimeWarrior : public ASlimeCharacter
{
	GENERATED_BODY()

public:
	ASlimeWarrior();

protected:
	// 전사 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warrior|Stat")
	float WarriorMaxHealth = 150.f;

	// 전사 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warrior|Stat")
	float WarriorMoveSpeed = 300.f;

	// 무적 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warrior|Ability")
	float InvincibleDuration = 3.f;

	// 고유 능력 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Warrior|Ability")
	float SpecialAbilityCooldown = 18.f;

	// 현재 무적 상태인지 여부
	bool bIsInvincible = false;

	// 현재 고유 능력을 사용할 수 있는지 여부
	bool bCanUseSpecialAbility = true;

	// 무적 종료 Timer
	FTimerHandle InvincibleTimerHandle;

	// 쿨타임 Timer
	FTimerHandle SpecialAbilityCooldownTimerHandle;

protected:
	virtual void BeginPlay() override;

	// 전사 고유 능력 - 무적
	virtual void UseSpecialAbility() override;

	// 무적 종료
	void EndInvincibility();

	// 고유 능력 쿨타임 종료
	void ResetSpecialAbilityCooldown();

	// 전사 무적 상태에 따른 피격 가능 여부
	virtual bool CanTakeDamageFromEnemy() const override;

	// 전사가 사용할 수 있는 업그레이드 목록 반환
	virtual TArray<EPlayerUpgradeType> GetAvailableUpgrades() const override;
};