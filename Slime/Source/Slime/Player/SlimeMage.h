// SlimeMage.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeCharacter.h"
#include "SlimeMage.generated.h"

UCLASS()
class SLIME_API ASlimeMage : public ASlimeCharacter
{
	GENERATED_BODY()

public:
	ASlimeMage();

protected:
	// 마법사 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Stat")
	float MageMaxHealth = 80.f;

	// 마법사 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Stat")
	float MageMoveSpeed = 500.f;

	// 반사 상태 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Ability")
	float ReflectDuration = 10.f;

	// 반사 지속시간 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mage|Upgrade")
	float ReflectDurationUpgradeAmount = 5.f;

	// 반사 Projectile 데미지 배율
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mage|Reflect")
	float ReflectDamageMultiplier = 1.5f;

	// 반사 데미지 배율 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mage|Upgrade")
	float ReflectDamageMultiplierUpgradeAmount = 0.25f;

	// 반사 능력 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Ability")
	float ReflectCooldown = 60.f;

	// 현재 반사 상태인지 여부
	bool bIsReflecting = false;

	// 현재 반사 능력을 사용할 수 있는지 여부
	bool bCanReflect = true;

	// 반사 종료 Timer
	FTimerHandle ReflectDurationTimerHandle;

	// 반사 쿨타임 Timer
	FTimerHandle ReflectCooldownTimerHandle;

protected:
	virtual void BeginPlay() override;

	// 마법사 고유 능력 - 투사체 반사
	virtual void UseSpecialAbility() override;

	// 반사 상태 종료
	void EndReflect();

	// 반사 쿨타임 종료
	void ResetReflectCooldown();

	// 마법사가 사용할 수 있는 업그레이드 목록 반환
	virtual TArray<EPlayerUpgradeType> GetAvailableUpgrades() const override;

public:
	// 현재 투사체 반사 상태인지 반환
	bool IsReflecting() const;

	float GetReflectDuration() const { return ReflectDuration; }

	float GetReflectDurationUpgradeAmount() const { return ReflectDurationUpgradeAmount; }

	float GetReflectDamageMultiplier() const { return ReflectDamageMultiplier; }

	float GetReflectDamageMultiplierUpgradeAmount() const { return ReflectDamageMultiplierUpgradeAmount; }

	// 반사 지속시간 증가
	void UpgradeReflectDuration();

	// 반사 데미지 배율 증가
	void UpgradeReflectDamage();
};