// SlimeProjectile.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeProjectile.generated.h"

class ASlimeEnemy;
class USphereComponent;

UCLASS()
class SLIME_API ASlimeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlimeProjectile();

	// 공격할 적을 지정하는 함수
	void SetTargetEnemy(ASlimeEnemy* NewTargetEnemy);

protected:
    // 충돌을 담당하는 루트 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    USphereComponent* CollisionComponent;

    // 투사체 외형을 표시하는 메시
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
    UStaticMeshComponent* ProjectileMesh;

    // 투사체 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float MoveSpeed = 50.0f;

    // 공격할 대상
    UPROPERTY()
    ASlimeEnemy* TargetEnemy;
protected:
	virtual void BeginPlay() override;
};
