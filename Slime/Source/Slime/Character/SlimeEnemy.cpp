// SlimeEnemy.cpp

#include "Slime/Character/SlimeEnemy.h"
#include "GameFramework/CharacterMovementComponent.h" // CharacterMovementComponent 사용
#include "Kismet/GameplayStatics.h"

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
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("%s 사망"),
            *GetName()
        );

        Destroy();
    }
}

