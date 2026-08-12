// SlimeArcher.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeCharacter.h"
#include "SlimeArcher.generated.h"

UCLASS()
class SLIME_API ASlimeArcher : public ASlimeCharacter
{
	GENERATED_BODY()

public:
	ASlimeArcher();

protected:
	// 궁수 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Archer|Stat")
	float ArcherMaxHealth = 100.f;

	// 궁수 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Archer|Stat")
	float ArcherMoveSpeed = 600.f;

	// 대쉬 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Archer|Ability")
	float DashSpeed = 5000.f;

	// 대쉬 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Archer|Ability")
	float DashCooldown = 5.f;

	// 대쉬 쿨타임 감소량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Archer|Upgrade")
	float DashCooldownUpgradeAmount = 0.25f;

	// 현재 대쉬를 사용할 수 있는지
	bool bCanDash = true;

	// 마지막으로 입력한 이동 방향
	FVector LastMoveDirection = FVector::ZeroVector;

	// 대쉬 쿨타임 Timer
	FTimerHandle DashCooldownTimerHandle;

protected:
	virtual void BeginPlay() override;

	// 궁수 고유 능력 - 대쉬
	virtual void UseSpecialAbility() override;

	// 대쉬 쿨타임 종료
	void ResetDashCooldown();

	// 궁수가 사용할 수 있는 업그레이드 목록 반환
	virtual TArray<EPlayerUpgradeType> GetAvailableUpgrades() const override;
	
public:
	// 현재 대쉬 쿨타임 반환
	float GetDashCooldown() const { return DashCooldown; }

	// 대쉬 쿨타임 감소량 반환
	float GetDashCooldownUpgradeAmount() const { return DashCooldownUpgradeAmount; }

	// 대쉬 쿨타임 감소
	void UpgradeDashCooldown();
};
