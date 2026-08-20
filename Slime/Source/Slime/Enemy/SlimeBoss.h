// SlimeBoss.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeBoss.generated.h"

UCLASS()
class SLIME_API ASlimeBoss : public ASlimeEnemy
{
	GENERATED_BODY()

public:
	ASlimeBoss();

protected:
	// Boss 전용 초기화
	virtual void BeginPlay() override;

	// Boss 사망 처리
	virtual void Die() override;

public:
	// Boss 현재 체력 반환
	UFUNCTION(BlueprintPure, Category = "Boss|UI")
	float GetBossCurrentHealth() const;

	// Boss 최대 체력 반환
	UFUNCTION(BlueprintPure, Category = "Boss|UI")
	float GetBossMaxHealth() const;

	// Boss HP 비율 반환
	UFUNCTION(BlueprintPure, Category = "Boss|UI")
	float GetBossHealthPercent() const;
};
