// SlimeEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlimeEnemy.generated.h"

class ASlimeExpOrbBase;

UCLASS()
class SLIME_API ASlimeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ASlimeEnemy();

protected:
	// 적의 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float MaxHealth = 30.f;

	// 적의 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stat")
	float CurrentHealth;

	// 경험치 종류 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASlimeExpOrbBase> ExpOrbClass;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Projectile에게 피해를 받는 함수
	void TakeDamageFromProjectile(float DamageAmount);
};
