// SlimeEnemy.cpp

#include "Slime/Character/SlimeEnemy.h"
#include "Slime/Item/SlimeExpOrbBase.h"
#include "Slime/Enemy/SlimeEnemySpawnManager.h"

#include "GameFramework/CharacterMovementComponent.h" // CharacterMovementComponent 사용
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

ASlimeEnemy::ASlimeEnemy()
{
	// Tick 함수 사용
	PrimaryActorTick.bCanEverTick = true;

	// 컨트롤러의 회전을 사용하지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동하는 방향을 바라보도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 적의 이동 속도 설정
	GetCharacterMovement()->MaxWalkSpeed = 250.f;

}

void ASlimeEnemy::BeginPlay()
{
	Super::BeginPlay();

    CurrentHealth = MaxHealth;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("%s HP 초기화 : %.1f / %.1f"),
        *GetName(),
        CurrentHealth,
        MaxHealth
    );
	
}

void ASlimeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 현재 월드의 첫 번째 플레이어 캐릭터를 가져온다.
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    // 플레이어를 정상적으로 찾지 못했다면 아래 코드를 실행하지 않는다.
    if (IsValid(PlayerCharacter) == false)
    {
        return;
    }

    // 몬스터에서 플레이어로 향하는 방향을 구한다.
    FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();

    // 위아래 높이 차이는 무시하고 바닥 방향으로만 이동한다.
    Direction.Z = 0.0f;

    // 방향 벡터의 길이를 1로 맞춘다.
    Direction.Normalize();

    // 계산한 방향으로 몬스터를 이동시킨다.
    AddMovementInput(Direction);
}

void ASlimeEnemy::TakeDamageFromProjectile(float DamageAmount)
{
    // 잘못된 피해량은 무시
    if (DamageAmount <= 0.f)
    {
        return;
    }

    // 현재 체력 감소
    CurrentHealth -= DamageAmount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("%s 피격! HP : %.1f / %.1f"),
        *GetName(),
        CurrentHealth,
        MaxHealth
    );

    // 체력이 0 이하가 되면 사망
    if (CurrentHealth <= 0.f)
    {
        // 드랍할 경험치 오브 클래스가 설정되어 있다면 생성
        if (ExpOrbClass)
        {
            // Enemy 캡슐의 절반 높이
            const float CapsuleHalfHeight =
                GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

            // Enemy 중심 위치에서 캡슐 절반 높이만큼 내려서 바닥 위치 계산
            FVector SpawnLocation = GetActorLocation();
            SpawnLocation.Z -= CapsuleHalfHeight;

            // 오브가 바닥에 조금 묻히지 않도록 살짝 위로 올림
            SpawnLocation.Z += 30.f;

            // 경험치 오브 생성
            ASlimeExpOrbBase* SpawnedExpOrb =
                GetWorld()->SpawnActor<ASlimeExpOrbBase>(
                    ExpOrbClass,
                    SpawnLocation,
                    FRotator::ZeroRotator
                );

            // 생성된 오브에 이 Enemy의 경험치 보상값 전달
            if (IsValid(SpawnedExpOrb))
            {
                SpawnedExpOrb->SetExpAmount(ExpReward);
            }
        }
        else
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("%s의 ExpOrbClass가 설정되지 않았습니다."),
                *GetName()
            );
        }

        // Enemy Spawn Manager 가져오기
        ASlimeEnemySpawnManager* SpawnManager =
            Cast<ASlimeEnemySpawnManager>(
                UGameplayStatics::GetActorOfClass(
                    this,
                    ASlimeEnemySpawnManager::StaticClass()
                )
            );

        // Spawn Manager가 유효하다면 처치 알림
        if (IsValid(SpawnManager))
        {
            SpawnManager->NotifyEnemyKilled();
        }

        // 경험치 오브 생성 후 Enemy 제거
        Destroy();
    }
}



