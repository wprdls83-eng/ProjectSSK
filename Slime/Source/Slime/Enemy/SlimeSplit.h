// SlimeSplit.h

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeSplit.generated.h"

UCLASS()
class SLIME_API ASlimeSplit : public ASlimeEnemy
{
	GENERATED_BODY()

public:
	ASlimeSplit();

protected:
	// 분열 후 생성할 작은 슬라임 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split")
	TSubclassOf<ASlimeEnemy> SplitEnemyClass;

	// 분열해서 생성할 Enemy 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split")
	int32 SplitCount = 2;

	// 분열된 Enemy 체력 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split")
	float SplitHealthRatio = 0.5f;

	// 분열된 Enemy 공격력 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split")
	float SplitDamageRatio = 0.5f;

	// 분열된 Enemy 크기 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split")
	float SplitScaleRatio = 0.7f;

	// 분열된 Enemy가 다시 분열하지 못하도록 구분
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Split")
	bool bCanSplit = true;

protected:
	// 사망 시 분열 처리
	virtual void Die() override;

	// 실제 분열 처리
	void SpawnSplitEnemies();
};
