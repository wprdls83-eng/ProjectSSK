// SlimeEnemyProjectile.H

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeEnemyProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ASlimeEnemy;
class UNiagaraSystem;
class UMaterialInterface;

UCLASS()
class SLIME_API ASlimeEnemyProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASlimeEnemyProjectile();

protected:
	// Projectile 충돌 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComponent;

	// Projectile 외형
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	// Projectile이 대상에게 명중했을 때 재생할 Niagara 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Effect")
	TObjectPtr<UNiagaraSystem> HitEffect;

	// 반사된 Projectile이 Enemy에게 명중했을 때 재생할 Niagara 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Effect")
	TObjectPtr<UNiagaraSystem> ReflectHitEffect;

	// 마법사에게 반사됐을 때 Projectile에 적용할 Material
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Reflect")
	TObjectPtr<UMaterialInterface> ReflectMaterial;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float MoveSpeed = 600.f;

	// 플레이어에게 입힐 데미지
	float Damage = 0.f;

	// 반사된 Projectile의 데미지 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Reflect")
	float ReflectDamageMultiplier = 1.5f;

	// 마법사에게 반사된 Projectile인지 여부
	bool bIsReflected = false;

	// 발사 순간 결정된 이동 방향
	FVector MoveDirection = FVector::ZeroVector;


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnProjectileOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 현재 위치에서 가장 가까운 Enemy 찾기
	ASlimeEnemy* FindNearestEnemy() const;

public:
	// 원거리 Enemy가 발사 방향 설정
	void SetMoveDirection(const FVector& NewDirection);

	// 원거리 Enemy가 데미지 설정
	void SetDamage(float NewDamage);
};