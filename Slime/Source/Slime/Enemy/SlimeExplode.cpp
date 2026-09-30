// SlimeExplode.cpp

#include "Slime/Enemy/SlimeExplode.h"
#include "Slime/Character/SlimeCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

ASlimeExplode::ASlimeExplode()
{

}

void ASlimeExplode::Tick(float DeltaTime)
{
	// 자폭 준비 중이면 경과 시간에 따라 점점 빨갛게 변경
	if (bIsSelfDestructing)
	{
		// 자폭 준비 경과 시간 증가
		SelfDestructElapsedTime += DeltaTime;

		// 0 ~ ExplosionDelay 시간을 0 ~ 1 값으로 변환
		const float WarningProgress =
			FMath::Clamp(
				SelfDestructElapsedTime / ExplosionDelay,
				0.f,
				1.f
			);

		// 초반에는 천천히, 폭발에 가까워질수록 빠르게 빨개짐
		const float WarningValue =
			FMath::Square(WarningProgress);

		// 기존 Enemy에서 생성한 Dynamic Material의
		// ExplodeWarning Parameter 값 변경
		if (IsValid(HitFlashMaterial))
		{
			HitFlashMaterial->SetScalarParameterValue(
				TEXT("ExplodeWarning"),
				WarningValue
			);
		}

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

	// 자폭 준비 알람 사운드 재생
	if (IsValid(CountdownSound))
	{
		CountdownAudioComponent =
			UGameplayStatics::SpawnSoundAtLocation(
				this,
				CountdownSound,
				GetActorLocation()
			);
	}

	// 자폭 준비가 시작되면 폭발 범위 표시
	SetExplosionWarningVisible(true);

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

	// 일정 시간 후 폭발
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&ASlimeExplode::ExplodeReady,
		ExplosionDelay,
		false
	);
}

void ASlimeExplode::ExplodeReady()
{
	// 3초 동안 재생하던 자폭 알람 정지
	if (IsValid(CountdownAudioComponent))
	{
		CountdownAudioComponent->Stop();
		CountdownAudioComponent = nullptr;
	}

	// 카운트다운이 끝난 순간 0초 알람 재생
	if (IsValid(ZeroSecondSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ZeroSecondSound,
			GetActorLocation()
		);
	}

	// 0초 알람 이후 2초 뒤 실제 폭발
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle,
		this,
		&ASlimeExplode::FinalExplode,
		1.f,
		false
	);
}

void ASlimeExplode::FinalExplode()
{
	// 실제 폭발 순간에는 범위 경고 제거
	SetExplosionWarningVisible(false);

	// 폭발 사운드 재생
	if (IsValid(ExplosionSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ExplosionSound,
			GetActorLocation()
		);
	}

	// 폭발 Niagara 이펙트 재생
	if (IsValid(ExplosionEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ExplosionEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

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
			// 기존 플레이어 피격 함수를 재사용하여 폭발 데미지 적용
			PlayerCharacter->TakeDamageFromEnemy(
				ExplosionDamage
			);
		}
	}

	// 폭발 처리 후 자폭병 제거
	FinishDeath();
}