// SlimeExpOrbBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlimeExpOrbBase.generated.h"

class USphereComponent;

UCLASS()
class SLIME_API ASlimeExpOrbBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASlimeExpOrbBase();

protected:
    // 플레이어에게 지급할 경험치
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exp Orb")
    int32 ExpAmount = 5;

    // 경험치 오브의 획득 충돌 범위
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exp Orb")
    TObjectPtr<USphereComponent> CollisionComponent;

    // 경험치 오브의 외형
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exp Orb")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // Enemy가 사망할 때 생성할 경험치 오브 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Drop")
    TSubclassOf<ASlimeExpOrbBase> ExpOrbClass;

protected:
	virtual void BeginPlay() override;

    // Overlap 함수
    UFUNCTION()
    void OnExpOrbOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );


};
