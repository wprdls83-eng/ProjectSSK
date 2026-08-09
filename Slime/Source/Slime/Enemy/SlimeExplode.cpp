// SlimeExplode.cpp

#include "Slime/Enemy/SlimeExplode.h"
#include "Slime/Character/SlimeCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASlimeExplode::ASlimeExplode()
{

}

void ASlimeExplode::Tick(float DeltaTime)
{
	// 이미 자폭 준비 중이면 더 이상 추적하지 않음
	if (bIsSelfDestructing)
	{
		return;
	}

	// 평소에는 부모의 플레이어 추적 실행
	Super::Tick(DeltaTime);

	// 플레이어 가져오기
	ACharacter* PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 플레이어와의 2D 거리 계산
	const float DistanceToPlayer =
		FVector::Dist2D(
			GetActorLocation(),
			PlayerCharacter->GetActorLocation()
		);

	// 자폭 거리 안으로 들어오면 자폭 준비 시작
	if (DistanceToPlayer <= SelfDestructRange)
	{
		StartSelfDestruct();
	}
}

void ASlimeExplode::Die()
{
	// 체력이 0이 되어도 바로 제거하지 않고
	// 자폭 준비 상태로 들어간다.
	StartSelfDestruct();
}

void ASlimeExplode::StartSelfDestruct()
{
	// 이미 자폭 준비 중이면 중복 실행 방지
	if (bIsSelfDestructing)
	{
		return;
	}

	bIsSelfDestructing = true;

	// 자폭 준비가 시작된 순간부터
	// 플레이어 무기의 공격 대상에서 제외
	bIsDead = true;

	// 이동 즉시 정지
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 죽은 상태에서는 더 이상 Projectile과 충돌하지 않도록 처리
	GetCapsuleComponent()->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s 자폭 준비 시작!"),
		*GetName()
	);

	// 일정 시간 후 폭발
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&ASlimeExplode::Explode,
		ExplosionDelay,
		false
	);
}

void ASlimeExplode::Explode()
{
	// 현재 플레이어 캐릭터 가져오기
	ASlimeCharacter* PlayerCharacter =
		Cast<ASlimeCharacter>(
			UGameplayStatics::GetPlayerCharacter(this, 0)
		);

	// 플레이어가 유효한 경우
	if (IsValid(PlayerCharacter))
	{
		// 자폭병과 플레이어 사이의 2D 거리 계산
		const float DistanceToPlayer =
			FVector::Dist2D(
				GetActorLocation(),
				PlayerCharacter->GetActorLocation()
			);

		// 플레이어가 폭발 범위 안에 있는 경우
		if (DistanceToPlayer <= ExplosionRadius)
		{
			// 기존 플레이어 피격 함수를 재사용하여
			// 자폭 데미지를 플레이어에게 적용
			PlayerCharacter->TakeDamageFromEnemy(
				ExplosionDamage
			);

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Player Explosion Hit! Damage : %.1f"),
				ExplosionDamage
			);
		}
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s 폭발!"),
		*GetName()
	);

	// 폭발 처리 후 자폭병 제거
	FinishDeath();
}