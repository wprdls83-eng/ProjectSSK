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
	// 이미 죽은 Enemy라면 추가 데미지를 받지 않음
	if (bIsDead)
	{
		return;
	}

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

	// 체력이 모두 소진되면 사망 처리
	if (CurrentHealth <= 0.f)
	{
		bIsDead = true;
		Die();
	}
}

bool ASlimeEnemy::IsDead() const
{
	return bIsDead;
}

void ASlimeEnemy::Die()
{
	FinishDeath();
}

void ASlimeEnemy::FinishDeath()
{
	// 경험치 오브 클래스가 설정되어 있다면 생성
	if (ExpOrbClass)
	{
		// Enemy 캡슐의 절반 높이
		const float CapsuleHalfHeight =
			GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		// Enemy의 바닥 위치 계산
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z -= CapsuleHalfHeight;

		// 오브가 바닥에 묻히지 않도록 조금 올림
		SpawnLocation.Z += 30.f;

		// 경험치 오브 생성
		GetWorld()->SpawnActor<ASlimeExpOrbBase>(
			ExpOrbClass,
			SpawnLocation,
			FRotator::ZeroRotator
		);
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

	// Spawn Manager에게 Enemy가 처치됐다고 알림
	if (IsValid(SpawnManager))
	{
		SpawnManager->NotifyEnemyKilled();
	}

	// Enemy 제거
	Destroy();
}
