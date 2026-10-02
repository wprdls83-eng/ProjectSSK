// SlimeBoss.cpp

#include "Slime/Enemy/SlimeBoss.h"
#include "Slime/Enemy/SlimeEnemySpawnManager.h"
#include "Slime/Character/SlimeCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"

ASlimeBoss::ASlimeBoss()
{
	// Boss 기본 체력
	MaxHealth = 500.f;

	// Boss 접촉 데미지
	AttackDamage = 30.f;

	// Boss 접촉 데미지 재적용 시간
	ContactDamageCooldown = 1.5f;

	// Boss 이동속도
	GetCharacterMovement()->MaxWalkSpeed = 180.f;

	// Boss Capsule에서 Hit Event 발생
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	// 돌진 충돌 처리 함수 연결
	GetCapsuleComponent()->OnComponentHit.AddDynamic(
		this,
		&ASlimeBoss::OnBossChargeHit
	);

	// Boss 근접 공격 Warning Mesh 생성
	MeleeWarningMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("MeleeWarningMesh")
		);

	// Boss Root에 부착
	MeleeWarningMesh->SetupAttachment(
		GetRootComponent()
	);

	// Warning Mesh는 충돌 판정에 사용하지 않음
	MeleeWarningMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	// 게임 시작 시에는 숨김
	MeleeWarningMesh->SetVisibility(false);

	// Boss 돌진 공격 Warning Mesh 생성
	ChargeWarningMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ChargeWarningMesh")
		);

	// Boss의 RootComponent에 부착
	ChargeWarningMesh->SetupAttachment(
		RootComponent
	);

	// 처음에는 돌진 Warning을 숨김
	ChargeWarningMesh->SetVisibility(false);

	// Warning Mesh는 충돌하지 않도록 설정
	ChargeWarningMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);
}

void ASlimeBoss::BeginPlay()
{
	Super::BeginPlay();

	// 근접 공격 Warning Material을 Dynamic Material로 생성
	MeleeWarningMaterial =
		MeleeWarningMesh->CreateAndSetMaterialInstanceDynamic(0);

	// 처음에는 Warning 진행도 0
	if (IsValid(MeleeWarningMaterial))
	{
		MeleeWarningMaterial->SetScalarParameterValue(
			TEXT("WarningProgress"),
			0.f
		);
	}

	// 평상시에는 Warning Mesh 숨김
	MeleeWarningMesh->SetVisibility(false);
}

void ASlimeBoss::Tick(float DeltaTime)
{	
	// 근접 공격 Warning이 표시 중이라면 진행도 증가
	if (
		IsValid(MeleeWarningMesh)
		&& MeleeWarningMesh->IsVisible()
		&& IsValid(MeleeWarningMaterial)
		)
	{
		// 지난 시간 누적
		MeleeWarningElapsedTime += DeltaTime;

		// 0 ~ 1 사이의 시간 진행도
		const float TimeProgress =
			FMath::Clamp(
				MeleeWarningElapsedTime / MeleeWarningTime,
				0.f,
				1.f
			);

		// Material의 원 반지름에 맞춰
		// 0 ~ 0.5 범위로 변환
		const float WarningProgress =
			TimeProgress * 0.5f;

		// Material의 WarningProgress 값 변경
		MeleeWarningMaterial->SetScalarParameterValue(
			TEXT("WarningProgress"),
			WarningProgress
		);
	}

	// 돌진 중이라면 저장해둔 방향으로 이동
	if (bIsCharging)
	{
		const FVector NewLocation =
			GetActorLocation()
			+ ChargeDirection
			* ChargeSpeed
			* DeltaTime;

		SetActorLocation(
			NewLocation,
			true
		);

		return;
	}

	// 공격 중이 아닐 때만 플레이어를 추적
	if (!bIsAttacking)
	{
		Super::Tick(DeltaTime);
	}

	// 공격 가능 여부 확인
	TryAttack();
}

void ASlimeBoss::Die()
{	
	// Boss가 사망하면 진행 중인 모든 공격 Timer 제거
	GetWorldTimerManager().ClearTimer(
		BossAttackCooldownTimerHandle
	);

	GetWorldTimerManager().ClearTimer(
		MeleeWarningTimerHandle
	);

	GetWorldTimerManager().ClearTimer(
		ChargeWarningTimerHandle
	);

	GetWorldTimerManager().ClearTimer(
		ChargeDurationTimerHandle
	);

    // 현재 월드의 Enemy Spawn Manager 가져오기
    ASlimeEnemySpawnManager* SpawnManager =
        Cast<ASlimeEnemySpawnManager>(
            UGameplayStatics::GetActorOfClass(
                this,
                ASlimeEnemySpawnManager::StaticClass()
            )
        );

	// 공격 및 돌진 상태 종료
	bIsAttacking = false;
	bIsCharging = false;

    // Spawn Manager에게 Boss가 처치됐다고 알림
    if (IsValid(SpawnManager))
    {
        SpawnManager->NotifyBossKilled();
    }

    // Boss 제거
    Destroy();
}

void ASlimeBoss::TryAttack()
{	
	// 현재 공격 중이라면 새로운 공격을 시도하지 않음
	if (bIsAttacking)
	{
		return;
	}

	// 이미 공격 쿨타임 중이라면 종료
	if (!bCanAttack)
	{
		return;
	}

	// 현재 플레이어 가져오기
	ACharacter* PlayerCharacter =
		UGameplayStatics::GetPlayerCharacter(
			this,
			0
		);

	// 플레이어가 없다면 종료
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// Boss와 플레이어 사이 거리 계산
	const float DistanceToPlayer =
		FVector::Dist2D(
			GetActorLocation(),
			PlayerCharacter->GetActorLocation()
		);

	// 공격 범위 밖이라면 공격하지 않음
	if (DistanceToPlayer > BossAttackRange)
	{
		return;
	}

	// 공격 실행
	PerformAttack();
}

void ASlimeBoss::PerformAttack()
{
	// 공격 쿨타임 시작
	bCanAttack = false;

	// 0 또는 1 중 랜덤 선택
	const int32 RandomPattern = FMath::RandRange(0, 1);

	// 근접 범위 공격
	if (RandomPattern == 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Boss Pattern : Melee Attack")
		);

		PerformMeleeAttack();
	}
	// 돌진 공격
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Boss Pattern : Charge Attack")
		);

		PerformChargeAttack();
	}

	// 두 공격이 같은 공용 쿨타임 사용
	GetWorldTimerManager().SetTimer(
		BossAttackCooldownTimerHandle,
		this,
		&ASlimeBoss::ResetAttackCooldown,
		BossAttackCooldown,
		false
	);
}

void ASlimeBoss::PerformMeleeAttack()
{	
	// 공격 예고부터 Boss 이동 정지
	bIsAttacking = true;

	// 근접 공격 Warning 표시
	MeleeWarningMesh->SetVisibility(true);

	// 근접 범위 공격 Warning이 시작될 때 경고음 재생
	if (IsValid(MeleeWarningSound))
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			MeleeWarningSound,
			GetActorLocation()
		);
	}

	// Warning 진행 시간 초기화
	MeleeWarningElapsedTime = 0.f;

	// Warning 진행도를 처음부터 시작
	if (IsValid(MeleeWarningMaterial))
	{
		MeleeWarningMaterial->SetScalarParameterValue(
			TEXT("WarningProgress"),
			0.f
		);
	}

	// 예고 시간이 지난 뒤 실제 공격 실행
	GetWorldTimerManager().SetTimer(
		MeleeWarningTimerHandle,
		this,
		&ASlimeBoss::ExecuteMeleeAttack,
		MeleeWarningTime,
		false
	);
}

void ASlimeBoss::ResetAttackCooldown()
{
	bCanAttack = true;
}

void ASlimeBoss::ExecuteMeleeAttack()
{	
	// 실제 근접 공격이 발동되면 Warning Mesh 숨김
	if (IsValid(MeleeWarningMesh))
	{
		MeleeWarningMesh->SetVisibility(false);
	}

	// 다음 공격을 위해 Warning 진행도 초기화
	if (IsValid(MeleeWarningMaterial))
	{
		MeleeWarningMaterial->SetScalarParameterValue(
			TEXT("WarningProgress"),
			0.f
		);
	}

	ASlimeCharacter* PlayerCharacter =
		Cast<ASlimeCharacter>(
			UGameplayStatics::GetPlayerCharacter(this, 0)
		);

	// Boss 근접 공격 발동 Niagara 이펙트 재생
	if (IsValid(MeleeAttackEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			MeleeAttackEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}

	if (IsValid(PlayerCharacter))
	{
		// 공격이 실제로 터지는 순간 거리 확인
		const float DistanceToPlayer =
			FVector::Dist2D(
				GetActorLocation(),
				PlayerCharacter->GetActorLocation()
			);

		// 플레이어가 아직 공격 범위 안에 있다면 데미지
		if (DistanceToPlayer <= MeleeAttackRadius)
		{
			PlayerCharacter->TakeDamageFromEnemy(
				MeleeAttackDamage
			);

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Boss Melee Attack Hit!")
			);
		}
		else
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Boss Melee Attack Miss!")
			);
		}
	}

	// 공격 종료
	// 다시 플레이어를 추적할 수 있음
	bIsAttacking = false;
}

void ASlimeBoss::PerformChargeAttack()
{
	// 현재 플레이어 가져오기
	ASlimeCharacter* PlayerCharacter =
		Cast<ASlimeCharacter>(
			UGameplayStatics::GetPlayerCharacter(
				this,
				0
			)
		);

	// 플레이어가 없다면 종료
	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 공격 준비 중에는 일반 추적 이동 정지
	bIsAttacking = true;

	// Boss에서 플레이어를 향하는 방향 계산
	ChargeDirection =
		PlayerCharacter->GetActorLocation()
		- GetActorLocation();

	// 바닥 방향으로만 돌진
	ChargeDirection.Z = 0.f;

	// 방향 벡터의 길이를 1로 맞춤
	ChargeDirection.Normalize();

	// 돌진 예고선 길이
	const float ChargeWarningLength =
		ChargeSpeed * ChargeDuration;
	
	// 돌진 Warning Mesh 설정
	if (IsValid(ChargeWarningMesh))
	{
		// 돌진 방향의 월드 회전값 계산
		const FRotator ChargeRotation =
			ChargeDirection.Rotation();

		// 먼저 Warning Mesh를 돌진 방향으로 회전
		ChargeWarningMesh->SetWorldRotation(
			FRotator(
				0.f,
				ChargeRotation.Yaw,
				0.f
			)
		);

		// Warning 표시
		ChargeWarningMesh->SetVisibility(true);

		// 돌진 예고 Niagara 이펙트 재생
		if (IsValid(ChargeEffect))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				ChargeEffect,
				GetActorLocation() + FVector(0.f, 0.f, 100.f),
				GetActorRotation()
			);
		}

		// 돌진 예고 효과음 재생
		if (IsValid(ChargeSound))
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				ChargeSound,
				GetActorLocation()
			);
		}
	}

	// 일정 시간 예고 후 실제 돌진 시작
	GetWorldTimerManager().SetTimer(
		ChargeWarningTimerHandle,
		this,
		&ASlimeBoss::StartCharge,
		ChargeWarningTime,
		false
	);
}

void ASlimeBoss::StartCharge()
{	
	// 돌진이 시작되면 Warning Mesh 숨김
	if (IsValid(ChargeWarningMesh))
	{
		ChargeWarningMesh->SetVisibility(false);
	}

	// 새로운 돌진이 시작되었으므로
	// 아직 플레이어에게 돌진 데미지를 주지 않은 상태로 초기화
	bHasDealtChargeDamage = false;

	// 돌진 시작
	bIsCharging = true;

	// 일정 시간이 지나면 돌진 종료
	GetWorldTimerManager().SetTimer(
		ChargeDurationTimerHandle,
		this,
		&ASlimeBoss::EndCharge,
		ChargeDuration,
		false
	);
}

void ASlimeBoss::OnBossChargeHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	// 현재 돌진 중이 아니라면 돌진 데미지를 주지 않음
	if (!bIsCharging)
	{
		return;
	}

	// 이번 돌진에서 이미 데미지를 줬다면 종료
	if (bHasDealtChargeDamage)
	{
		return;
	}

	// 충돌한 Actor가 플레이어인지 확인
	ASlimeCharacter* PlayerCharacter =
		Cast<ASlimeCharacter>(OtherActor);

	if (!IsValid(PlayerCharacter))
	{
		return;
	}

	// 플레이어에게 돌진 데미지 적용
	PlayerCharacter->TakeDamageFromEnemy(
		ChargeDamage
	);

	// 이번 돌진에서는 더 이상 데미지를 주지 않음
	bHasDealtChargeDamage = true;
}

void ASlimeBoss::EndCharge()
{
	// 돌진 종료
	bIsCharging = false;

	// 공격 상태 종료
	bIsAttacking = false;
}

void ASlimeBoss::CheckPhaseTwo()
{
	// 이미 2페이즈라면 종료
	if (bIsPhaseTwo)
	{
		return;
	}

	// Boss 체력이 설정한 비율 이하인지 확인
	if (GetBossHealthPercent() <= PhaseTwoHealthPercent)
	{
		EnterPhaseTwo();
	}
}

void ASlimeBoss::EnterPhaseTwo()
{
	// 이미 2페이즈라면 중복 실행 방지
	if (bIsPhaseTwo)
	{
		return;
	}

	// 2페이즈 상태로 변경
	bIsPhaseTwo = true;

	// 공격 간격 감소
	BossAttackCooldown *=
		PhaseTwoAttackCooldownMultiplier;

	// 돌진 속도 증가
	ChargeSpeed *=
		PhaseTwoChargeSpeedMultiplier;

	// 근접 공격 예고시간 감소
	MeleeWarningTime *=
		PhaseTwoMeleeWarningMultiplier;

	// 2페이즈 진입 로그
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Boss Phase 2 Start!")
	);

	// 2페이즈 진입을 화면에서 확인하기 위한 테스트 메시지
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Red,
			TEXT("BOSS PHASE 2!")
		);
	}

}

void ASlimeBoss::OnHealthChanged()
{
	// 부모의 체력 변경 처리 실행
	Super::OnHealthChanged();

	// Boss의 2페이즈 진입 조건 확인
	CheckPhaseTwo();
}

float ASlimeBoss::GetBossCurrentHealth() const
{
	return CurrentHealth;
}

float ASlimeBoss::GetBossMaxHealth() const
{
	return MaxHealth;
}

float ASlimeBoss::GetBossHealthPercent() const
{
	// 0으로 나누는 것을 방지
	if (MaxHealth <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(
		CurrentHealth / MaxHealth,
		0.f,
		1.f
	);
}

float ASlimeBoss::GetContactDamage() const
{
	// 돌진 중에는 별도의 ChargeDamage를 사용하므로
	// 일반 접촉 데미지는 적용하지 않음
	if (bIsCharging)
	{
		return 0.f;
	}

	// 평상시에는 Boss의 기본 접촉 데미지 사용
	return AttackDamage;
}
