// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slime/Character/SlimeEnemy.h"
#include "SlimeCharge.generated.h"

UCLASS()
class SLIME_API ASlimeCharge : public ASlimeEnemy
{
	GENERATED_BODY()
	
public:
	ASlimeCharge();

protected:
	// 플레이어가 이 거리 안에 들어오면 돌진 준비 시작
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeDetectRange = 600.f;

	// 돌진하기 전 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeReadyTime = 2.5f;

	// 돌진할 때 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeSpeed = 1200.f;

	// 돌진 중 플레이어와 충돌했을 때의 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeDamage = 30.f;

	// 돌진할 때 사용할 가속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeAcceleration = 10000.f;

	// 실제 돌진이 유지되는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeDuration = 0.6f;

	// 돌진 후 다시 돌진할 수 있을 때까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeCooldown = 8.f;

	// 돌진 후 잠시 멈춰있는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charge")
	float ChargeRecoveryTime = 1.5f;

	// 돌진 종료 후 복구할 기본 이동속도
	float OriginalMoveSpeed = 0.f;

	// 돌진 종료 후 복구할 기본 가속도
	float OriginalMaxAcceleration = 0.f;

	// 현재 돌진 준비 중인지 여부
	bool bIsPreparingCharge = false;

	// 현재 돌진 중인지 여부
	bool bIsCharging = false;

	// 돌진 후 회복 중인지 여부
	bool bIsRecovering = false;

	// 현재 돌진 가능 상태인지 여부
	bool bCanCharge = true;

	// 돌진 방향
	FVector ChargeDirection = FVector::ZeroVector;

	// 돌진 준비 Timer
	FTimerHandle ChargeReadyTimerHandle;

	// 돌진 쿨타임 Timer
	FTimerHandle ChargeCooldownTimerHandle;

	// 돌진 종료 Timer
	FTimerHandle ChargeDurationTimerHandle;

	// 돌진 후 회복 Timer
	FTimerHandle ChargeRecoveryTimerHandle;

protected:
	virtual void Tick(float DeltaTime) override;

	// 현재 상태에 따른 접촉 데미지 반환
	virtual float GetContactDamage() const override;

public:
	// 돌진 시작
	void StartCharge();

	// 돌진 종료
	void EndCharge();

	// 돌진 후 정지 상태 종료
	void EndChargeRecovery();

	// 쿨타임 종료 후 다시 돌진 가능
	void ResetChargeCooldown();
};

