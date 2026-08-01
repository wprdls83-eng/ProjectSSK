// SlimeEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlimeEnemy.generated.h"

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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Projectile에게 피해를 받는 함수
	void TakeDamageFromProjectile(float DamageAmount);
};
