// SlimeMageProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeMageProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class SLIME_API ASlimeMageProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASlimeMageProjectile();

protected:
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mage|Projectile")
	TObjectPtr<USphereComponent> CollisionComponent;

	// Projectile 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mage|Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	// 낙하 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Projectile")
	float FallSpeed = 1000.f;

	// 폭발 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mage|Projectile")
	float ExplosionRadius = 200.f;

	// 폭발 데미지
	float Damage = 0.f;

	// 목표 지면 위치
	FVector TargetLocation = FVector::ZeroVector;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 도착 시 범위 데미지 발생
	void Explode();


public:
	// 목표 지점 설정
	void SetTargetLocation(const FVector& NewTargetLocation);

	// Weapon에서 데미지 전달
	void SetDamage(float NewDamage);

	// Weapon에서 폭발 범위를 전달받는 함수
	void SetExplosionRadius(float NewExplosionRadius);
};
