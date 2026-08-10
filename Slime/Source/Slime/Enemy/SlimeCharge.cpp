// Fill out your copyright notice in the Description page of Project Settings.

#include "Slime/Enemy/SlimeCharge.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ASlimeCharge::ASlimeCharge()
{

}

void ASlimeCharge::Tick(float DeltaTime)
{
	// 실제 돌진 중일 때
	if (bIsCharging)
	{
		// 저장해둔 방향으로만 이동
		AddMovementInput(ChargeDirection);

		// 부모 Tick을 호출하지 않고 바로 종료
		return;
	}

	// 돌진 준비 중이면 완전히 정지
	if (bIsPreparingCharge)
	{
		return;
	}

	// 돌진 후 회복 중이면 완전히 정지
	if (bIsRecovering)
	{
		return;
	}

	// 위 상태가 아닐 때만 부모의 일반 추적 실행
	Super::Tick(DeltaTime);

	// 쿨타임 중이라면 돌진 검사하지 않음
	if (!bCanCharge)
	{
		return;
	}

	// 플레이어 가져오기
	ACharacter* PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 플레이어와 거리 계산
	const float DistanceToPlayer =
		FVector::Dist2D(
			GetActorLocation(),
			PlayerCharacter->GetActorLocation()
		);

	// 돌진 범위 밖이면 종료
	if (DistanceToPlayer > ChargeDetectRange)
	{
		return;
	}

	// 돌진 준비 시작
	bIsPreparingCharge = true;

	// 돌진 준비가 시작되는 순간의 플레이어 방향을 저장
	ChargeDirection =
		PlayerCharacter->GetActorLocation()
		- GetActorLocation();

	// 높이 차이는 무시
	ChargeDirection.Z = 0.f;

	// 방향 벡터 정규화
	ChargeDirection.Normalize();

	GetWorldTimerManager().SetTimer(
		ChargeReadyTimerHandle,
		this,
		&ASlimeCharge::StartCharge,
		ChargeReadyTime,
		false
	);
}

void ASlimeCharge::StartCharge()
{
	// 현재 이동속도 저장
	OriginalMoveSpeed =
	GetCharacterMovement()->MaxWalkSpeed;

	// 현재 가속도 저장
	OriginalMaxAcceleration =
	GetCharacterMovement()->MaxAcceleration;

	// 돌진 속도로 변경
	GetCharacterMovement()->MaxWalkSpeed =
	ChargeSpeed;

	// 돌진 시작과 동시에 빠르게 최고속도에 도달하도록 가속도 증가
	GetCharacterMovement()->MaxAcceleration =
	ChargeAcceleration;

	// 준비 상태 종료
	bIsPreparingCharge = false;

	// 실제 돌진 상태 시작
	bIsCharging = true;

	// 돌진 재사용 불가
	bCanCharge = false;

	// 일정 시간이 지나면 돌진 종료
	GetWorldTimerManager().SetTimer(
		ChargeDurationTimerHandle,
		this,
		&ASlimeCharge::EndCharge,
		ChargeDuration,
		false
	);
}

void ASlimeCharge::ResetChargeCooldown()
{
	bCanCharge = true;
}

void ASlimeCharge::EndCharge()
{
	// 돌진 종료
	bIsCharging = false;

	// 원래 이동속도로 복구
	GetCharacterMovement()->MaxWalkSpeed =
	OriginalMoveSpeed;

	// 원래 가속도로 복구
	GetCharacterMovement()->MaxAcceleration =
	OriginalMaxAcceleration;

	// 돌진 후 잠시 정지 상태
	bIsRecovering = true;

	// 일정 시간이 지나면 다시 추적 시작
	GetWorldTimerManager().SetTimer(
		ChargeRecoveryTimerHandle,
		this,
		&ASlimeCharge::EndChargeRecovery,
		ChargeRecoveryTime,
		false
	);

	// 돌진 쿨타임 시작
	GetWorldTimerManager().SetTimer(
		ChargeCooldownTimerHandle,
		this,
		&ASlimeCharge::ResetChargeCooldown,
		ChargeCooldown,
		false
	);
}

float ASlimeCharge::GetContactDamage() const
{
	// 실제 돌진 중이면 돌진 전용 데미지
	if (bIsCharging)
	{
		return ChargeDamage;
	}

	// 평상시에는 부모의 일반 접촉 데미지
	return Super::GetContactDamage();
}

void ASlimeCharge::EndChargeRecovery()
{
	bIsRecovering = false;
}
