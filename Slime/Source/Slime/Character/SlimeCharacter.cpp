// SlimeCharacter.cpp

#include "Slime/Character/SlimeCharacter.h"
#include "Slime/Character/SlimeEnemy.h"
#include "Slime/Player/SlimeArcher.h"
#include "Slime/Player/SlimeMage.h"
#include "Slime/Weapons/SlimeWeaponBase.h"
#include "Slime/Weapons/SlimeWarriorWeapon.h"
#include "Slime/Weapons/SlimeMageWeapon.h"
#include "Slime/UI/SlimePlayerHUDWidget.h"
#include "Slime/UI/SlimeGameOverWidget.h"

#include "Camera/CameraComponent.h" 
#include "GameFramework/SpringArmComponent.h" 
#include "GameFramework/CharacterMovementComponent.h" 
#include "Blueprint/UserWidget.h" 
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

ASlimeCharacter::ASlimeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 아직 처리하지 않은 레벨업 없음
	PendingLevelUps = 0;

	// 컨트롤러 회전을 사용하지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동하는 방향을 바라보도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// 회전 속도
	GetCharacterMovement()->RotationRate = FRotator(0.f, 50.f, 0.f);

	// SpringArm 컴포넌트 생성
	SpringArmComp =	CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	// 루트(Capsule)에 부착
	SpringArmComp->SetupAttachment(RootComponent);

	// 플레이어 회전과 카메라 회전을 분리
	SpringArmComp->SetUsingAbsoluteRotation(true);

	// 카메라와 캐릭터 사이 거리
	SpringArmComp->TargetArmLength = 900.f;

	// 고정 카메라 각도
	SpringArmComp->SetWorldRotation(
		FRotator(-55.f, 0.f, 0.f)
	);

	// 위에서 비스듬히 내려다보는 각도
	SpringArmComp->SetRelativeRotation(FRotator(-55.f, 0.f, 0.f));

	// 벽에 닿아도 카메라가 당겨지지 않도록 설정
	SpringArmComp->bDoCollisionTest = false;

	// 컨트롤러 회전을 사용하지 않음
	SpringArmComp->bUsePawnControlRotation = false;

	// Camera 컴포넌트 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	// SpringArm 끝에 Camera를 부착
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	// 컨트롤러 회전을 사용하지 않음
	CameraComp->bUsePawnControlRotation = false;
}

float ASlimeCharacter::GetMoveSpeed() const
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void ASlimeCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 직업 선택 화면용 Preview 캐릭터라면 게임 플레이용 초기화는 실행하지 않음
	if (bIsPreviewCharacter)
	{
		return;
	}

	// 현재 체력을 최대 체력으로 초기화
	CurrentHealth = MaxHealth;

	// 무기 클래스가 설정되지 않았다면 종료
	if (!WeaponClass)
	{
		return;
	}

	// 생성할 무기의 Owner와 Instigator를 플레이어로 설정
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;

	// 플레이어 위치에 무기 생성
	EquippedWeapon = GetWorld()->SpawnActor<ASlimeWeaponBase>(
		WeaponClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParameters
	);

	if (!IsValid(EquippedWeapon))
	{
		return;
	}

	// 생성된 무기를 플레이어에게 부착
	EquippedWeapon->AttachToComponent(
		GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);

	EquippedWeapon->SetActorRelativeLocation(
		FVector(0.f, 0.f, 50.f)
	);
}

void ASlimeCharacter::InitializePlayer()
{
	// 현재 캐릭터를 조종하는 PlayerController 가져오기
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	// Enhanced Input 설정
	ULocalPlayer* LocalPlayer =
		PlayerController->GetLocalPlayer();

	if (IsValid(LocalPlayer))
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

		if (IsValid(InputSubsystem) && PlayerMappingContext)
		{
			InputSubsystem->AddMappingContext(
				PlayerMappingContext,
				0
			);
		}
	}

	// Player HUD 생성
	if (PlayerHUDWidgetClass)
	{
		PlayerHUDWidget =
			CreateWidget<USlimePlayerHUDWidget>(
				PlayerController,
				PlayerHUDWidgetClass
			);

		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();

			// 현재 데이터로 HUD 초기화
			UpdatePlayerHUD();
		}
	}
}

void ASlimeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 가장 가까운 Enemy를 계속 바라봄
	LookAtNearestEnemy();
}

void ASlimeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 기본 InputComponent를 EnhancedInputComponent로 변환한다.
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (IsValid(EnhancedInputComponent) == false)
	{
		return;
	}

	// IA_Move 입력이 발생하는 동안 Move 함수를 호출한다.
	if (IsValid(MoveAction) == true)
	{
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ASlimeCharacter::Move);
	}

	// IA_SpecialAbility 입력 시 직업 고유 능력 사용
	if (IsValid(SpecialAbilityAction))
	{
		EnhancedInputComponent->BindAction(
			SpecialAbilityAction,
			ETriggerEvent::Started,
			this,
			&ASlimeCharacter::UseSpecialAbility
		);
	}
}

void ASlimeCharacter::Move(const FInputActionValue& Value)
{
	// 사망한 플레이어는 이동할 수 없음
	if (bIsDead)
	{
		return;
	}

	// IA_Move의 2D 입력값
	const FVector2D MoveInput =
		Value.Get<FVector2D>();

	// 월드 기준 이동 방향 계산
	CurrentMoveDirection = FVector(
		MoveInput.Y,
		MoveInput.X,
		0.f
	);

	// 대각선 이동에서도 방향 크기를 1로 유지
	CurrentMoveDirection =
		CurrentMoveDirection.GetSafeNormal();

	// 계산된 방향으로 이동
	if (!CurrentMoveDirection.IsNearlyZero())
	{
		AddMovementInput(
			CurrentMoveDirection,
			1.f
		);
	}
}

void ASlimeCharacter::UseSpecialAbility()
{
	// 구현은 자식에서
}

void ASlimeCharacter::AddExp(int32 ExpAmount)
{	
	// 경험치가 0보다 작거나 같으면 함수 실행x
	if (ExpAmount <= 0)
	{
		return;
	}

	// 경험치 증가
	CurrentExp += ExpAmount;

	// HUD 불러오기
	UpdatePlayerHUD();

	// 필요한 경험치 이상이면 레벨업
	while (CurrentExp >= NeedExp)
	{
		LevelUp();
	}
}

void ASlimeCharacter::LevelUp()
{
	// 레벨업에 필요한 경험치 차감
	CurrentExp -= NeedExp;

	// 플레이어 레벨 증가
	PlayerLevel++;

	// 다음 레벨 필요 경험치 증가
	NeedExp = FMath::RoundToInt(
		NeedExp * 1.5f
	);

	// 선택해야 할 업그레이드 횟수 증가
	PendingLevelUps++;

	// HUD 불러오기
	UpdatePlayerHUD();

	// 첫 번째 미처리 레벨업일 때만 UI 표시
	if (PendingLevelUps == 1)
	{
		ShowLevelUpUI();
	}

}

void ASlimeCharacter::ShowLevelUpUI()
{	
	// 이번 레벨업에서 보여줄 업그레이드 3개 생성
	GenerateUpgradeChoices();

	// 레벨업 UI 클래스가 설정되지 않았다면 생성 불가
	if (!LevelUpWidgetClass)
	{
		return;
	}

	// 현재 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	// UI가 아직 생성되지 않았다면 한 번만 생성
	if (!IsValid(LevelUpWidget))
	{
		LevelUpWidget = CreateWidget<UUserWidget>(
			PlayerController,
			LevelUpWidgetClass
		);

		if (!IsValid(LevelUpWidget))
		{
			return;
		}
	}

	// 화면에서 제거된 기존 UI를 다시 표시
	if (!LevelUpWidget->IsInViewport())
	{
		LevelUpWidget->AddToViewport();
	}

	// 게임 일시정지
	UGameplayStatics::SetGamePaused(this, true);

	// 마우스 커서 표시
	PlayerController->bShowMouseCursor = true;

	// UI만 입력받도록 변경
	FInputModeUIOnly InputMode;
	PlayerController->SetInputMode(InputMode);
}

void ASlimeCharacter::CompleteLevelUpSelection()
{
	// 처리할 레벨업이 없다면 종료
	if (PendingLevelUps <= 0)
	{
		return;
	}

	// 업그레이드 하나를 선택했으므로 횟수 감소
	PendingLevelUps--;

	// 아직 선택할 업그레이드가 남아 있다면
	// UI와 게임 일시정지 상태를 그대로 유지
	if (PendingLevelUps > 0)
	{
		return;
	}

	// 모든 업그레이드 선택이 끝났다면 UI 제거
	if (IsValid(LevelUpWidget))
	{
		LevelUpWidget->RemoveFromParent();
	}

	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	// 마우스 커서 숨기기
	PlayerController->bShowMouseCursor = false;

	// 입력을 게임 전용으로 변경
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	// 게임 일시정지 해제
	UGameplayStatics::SetGamePaused(
		this,
		false
	);
}

void ASlimeCharacter::UpgradeDamage()
{
	// 무기가 없다면 종료
	if (!IsValid(EquippedWeapon))
	{
		return;
	}

	// 무기에게 데미지 업그레이드를 요청
	EquippedWeapon->UpgradeDamage();
}

void ASlimeCharacter::UpgradeAttackSpeed()
{
	// 무기가 없다면 종료
	if (!IsValid(EquippedWeapon))
	{
		return;
	}

	// 무기에게 공격 속도 업그레이드를 요청
	EquippedWeapon->UpgradeAttackSpeed();
}

void ASlimeCharacter::UpgradeProjectileCount()
{
	// 무기가 없다면 종료
	if (!IsValid(EquippedWeapon))
	{
		return;
	}

	// 무기에게 발사체 개수 업그레이드를 요청
	EquippedWeapon->UpgradeProjectileCount();
}

void ASlimeCharacter::TakeDamageFromEnemy(float DamageAmount)
{
	// 이미 죽은 플레이어라면 추가 데미지를 받지 않음
	if (bIsDead)
	{
		return;
	}

	// 현재 데미지를 받을 수 없는 상태라면 종료
	if (!CanTakeDamageFromEnemy())
	{
		return;
	}

	// 잘못된 데미지는 처리하지 않음
	if (DamageAmount <= 0.f)
	{
		return;
	}

	// 현재 체력 감소
	CurrentHealth -= DamageAmount;

	// 체력이 0보다 작아지지 않도록 제한
	CurrentHealth = FMath::Max(0.f, CurrentHealth);

	// 변경된 체력을 HUD에 반영
	UpdatePlayerHUD();

	// 체력이 모두 소진되었다면 사망 처리
	if (CurrentHealth <= 0.f)
	{
		Die();
	}
}

bool ASlimeCharacter::CanTakeDamageFromEnemy() const
{
	// 기본 캐릭터는 데미지를 받을 수 있음
	return true;
}

void ASlimeCharacter::Die()
{
	// 이미 죽은 상태라면 중복 실행 방지
	if (bIsDead)
	{
		return;
	}

	// 사망 상태로 변경
	bIsDead = true;

	// 이동 즉시 정지
	GetCharacterMovement()->StopMovementImmediately();

	// 더 이상 이동하지 못하도록 Movement 비활성화
	GetCharacterMovement()->DisableMovement();

	// 장착 중인 무기가 있다면 자동 공격 중지
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->StopAttackTimer();
	}

	// Game Over UI 표시
	ShowGameOverUI();
}

void ASlimeCharacter::UpdatePlayerHUD()
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	PlayerHUDWidget->UpdateHUD(
		CurrentHealth,
		MaxHealth,
		CurrentExp,
		NeedExp,
		PlayerLevel
	);
}

void ASlimeCharacter::ShowGameOverUI()
{
	// Game Over Widget 클래스가 설정되지 않았다면 종료
	if (!GameOverWidgetClass)
	{
		return;
	}

	// PlayerController 가져오기
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	// Game Over Widget 생성
	GameOverWidget =
		CreateWidget<USlimeGameOverWidget>(
			PlayerController,
			GameOverWidgetClass
		);

	if (!IsValid(GameOverWidget))
	{
		return;
	}

	// 화면에 표시
	GameOverWidget->AddToViewport();

	// 마우스 커서 표시
	PlayerController->bShowMouseCursor = true;

	// UI만 입력받도록 변경
	FInputModeUIOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	// 게임 일시정지
	UGameplayStatics::SetGamePaused(
		this,
		true
	);
}

void ASlimeCharacter::ApplyEarlyClearReward(EEarlyClearRewardType RewardType)
{
	switch (RewardType)
	{
	case EEarlyClearRewardType::Damage:
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeDamage();
		}
		break;

	case EEarlyClearRewardType::AttackSpeed:
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeAttackSpeed();
		}
		break;

	case EEarlyClearRewardType::AttackRange:
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeAttackRange();
		}
		break;

	case EEarlyClearRewardType::MaxHealth:
		MaxHealth += 20.f;

		// 최대 체력이 늘어난 만큼 현재 체력도 같이 증가
		CurrentHealth += 20.f;

		UpdatePlayerHUD();
		break;

	case EEarlyClearRewardType::MoveSpeed:
		GetCharacterMovement()->MaxWalkSpeed += 30.f;
		break;

	case EEarlyClearRewardType::Exp:
		AddExp(50);
		break;
	}
}

void ASlimeCharacter::LookAtNearestEnemy()
{
	ASlimeEnemy* NearestEnemy = nullptr;

	float NearestDistanceSquared =
		TargetDetectRange * TargetDetectRange;

	for (TActorIterator<ASlimeEnemy> It(GetWorld()); It; ++It)
	{
		ASlimeEnemy* Enemy = *It;

		if (!IsValid(Enemy))
		{
			continue;
		}

		if (Enemy->IsDead())
		{
			continue;
		}

		const float DistanceSquared =
			FVector::DistSquared2D(
				GetActorLocation(),
				Enemy->GetActorLocation()
			);

		if (DistanceSquared > NearestDistanceSquared)
		{
			continue;
		}

		NearestDistanceSquared = DistanceSquared;
		NearestEnemy = Enemy;
	}

	if (!IsValid(NearestEnemy))
	{
		return;
	}

	FVector LookDirection =
		NearestEnemy->GetActorLocation()
		- GetActorLocation();

	LookDirection.Z = 0.f;

	if (LookDirection.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(
		LookDirection.Rotation()
	);
}

void ASlimeCharacter::GenerateUpgradeChoices()
{
	// 이전 레벨업 선택지 제거
	CurrentUpgradeChoices.Empty();

	// 현재 직업에서 등장 가능한 업그레이드 목록
	TArray<EPlayerUpgradeType> AvailableUpgrades =
		GetAvailableUpgrades();

	// 최대 3개까지만 선택
	const int32 ChoiceCount =
		FMath::Min(3, AvailableUpgrades.Num());

	for (int32 i = 0; i < ChoiceCount; ++i)
	{
		// 남아 있는 후보 중 하나를 랜덤 선택
		const int32 RandomIndex =
			FMath::RandRange(
				0,
				AvailableUpgrades.Num() - 1
			);

		// 선택된 업그레이드를 현재 선택지에 추가
		CurrentUpgradeChoices.Add(
			AvailableUpgrades[RandomIndex]
		);

		// 같은 업그레이드가 다시 선택되지 않도록 후보에서 제거
		AvailableUpgrades.RemoveAt(
			RandomIndex
		);
	}
}

TArray<EPlayerUpgradeType> ASlimeCharacter::GetCurrentUpgradeChoices() const
{
	return CurrentUpgradeChoices;
}

FText ASlimeCharacter::GetUpgradeDisplayName(
	EPlayerUpgradeType UpgradeType
) const
{
	switch (UpgradeType)
	{
	case EPlayerUpgradeType::Damage:
		return FText::FromString(TEXT("공격력 증가"));

	case EPlayerUpgradeType::AttackSpeed:
		return FText::FromString(TEXT("공격 속도 증가"));

	case EPlayerUpgradeType::AttackRange:
		return FText::FromString(TEXT("공격 범위 증가"));

	case EPlayerUpgradeType::MaxHealth:
		return FText::FromString(TEXT("최대 체력 증가"));

	case EPlayerUpgradeType::MoveSpeed:
		return FText::FromString(TEXT("이동 속도 증가"));

	case EPlayerUpgradeType::WarriorMaxHitEnemies:
		return FText::FromString(TEXT("최대 타격 수 증가"));

	case EPlayerUpgradeType::WarriorAttackAngle:
		return FText::FromString(TEXT("공격 각도 증가"));

	case EPlayerUpgradeType::ArcherProjectileCount:
		return FText::FromString(TEXT("투사체 수 증가"));

	case EPlayerUpgradeType::ArcherDashCooldown:
		return FText::FromString(TEXT("대쉬 쿨타임 감소"));

	case EPlayerUpgradeType::MageExplosionRadius:
		return FText::FromString(TEXT("폭발 범위 증가"));

	case EPlayerUpgradeType::MageReflectDuration:
		return FText::FromString(TEXT("반사 지속시간 증가"));

	case EPlayerUpgradeType::MageReflectDamage:
		return FText::FromString(TEXT("반사 피해 증가"));

	default:
		return FText::FromString(TEXT("알 수 없는 업그레이드"));
	}
}

FText ASlimeCharacter::GetUpgradeDescription(
	EPlayerUpgradeType UpgradeType
) const
{
	if (!IsValid(EquippedWeapon))
	{
		return FText::GetEmpty();
	}

	switch (UpgradeType)
	{
	case EPlayerUpgradeType::Damage:
	{
		const float Current =
			EquippedWeapon->GetDamage();

		const float Amount =
			EquippedWeapon->GetDamageUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("공격력 %.0f → %.0f (+%.0f)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::AttackSpeed:
	{
		const float Current =
			EquippedWeapon->GetAttackInterval();

		const float Amount =
			EquippedWeapon->GetAttackIntervalUpgradeAmount();

		const float Result =
			FMath::Max(
				0.1f,
				Current - Amount
			);

		return FText::FromString(
			FString::Printf(
				TEXT("공격 간격 %.1f초 → %.1f초"),
				Current,
				Result
			)
		);
	}

	case EPlayerUpgradeType::AttackRange:
	{
		const float Current =
			EquippedWeapon->GetAttackRange();

		const float Amount =
			EquippedWeapon->GetAttackRangeUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("공격 범위 %.0f → %.0f (+%.0f)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::MaxHealth:
	{
		const float Current = GetMaxHealth();
		const float Amount = GetMaxHealthUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("최대 체력 %.0f → %.0f (+%.0f)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::MoveSpeed:
	{
		const float Current = GetMoveSpeed();
		const float Amount = GetMoveSpeedUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("이동 속도 %.0f → %.0f (+%.0f)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::WarriorMaxHitEnemies:
	{
		ASlimeWarriorWeapon* WarriorWeapon =
			Cast<ASlimeWarriorWeapon>(EquippedWeapon);

		if (!IsValid(WarriorWeapon))
		{
			return FText::GetEmpty();
		}

		const int32 Current =
			WarriorWeapon->GetMaxHitEnemies();

		const int32 Amount =
			WarriorWeapon->GetMaxHitEnemiesUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("최대 타격 수 %d → %d (+%d)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::WarriorAttackAngle:
	{
		ASlimeWarriorWeapon* WarriorWeapon =
			Cast<ASlimeWarriorWeapon>(EquippedWeapon);

		if (!IsValid(WarriorWeapon))
		{
			return FText::GetEmpty();
		}

		const float Current =
			WarriorWeapon->GetAttackAngle();

		const float Amount =
			WarriorWeapon->GetAttackAngleUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("공격 각도 %.0f° → %.0f° (+%.0f°)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::ArcherProjectileCount:
	{
		const int32 Current =
			EquippedWeapon->GetProjectileCount();

		const int32 Amount =
			EquippedWeapon->GetProjectileCountUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("투사체 수 %d → %d (+%d)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::ArcherDashCooldown:
	{
		const ASlimeArcher* Archer =
			Cast<ASlimeArcher>(this);

		if (!IsValid(Archer))
		{
			return FText::GetEmpty();
		}

		const float Current =
			Archer->GetDashCooldown();

		const float Amount =
			Archer->GetDashCooldownUpgradeAmount();

		// 쿨타임이 음수가 되지 않도록 방지
		const float Result =
			FMath::Max(
				0.f,
				Current - Amount
			);

		return FText::FromString(
			FString::Printf(
				TEXT("대쉬 쿨타임 %.1f초 → %.1f초 (-%.1f초)"),
				Current,
				Result,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::MageExplosionRadius:
	{
		ASlimeMageWeapon* MageWeapon =
			Cast<ASlimeMageWeapon>(EquippedWeapon);

		if (!IsValid(MageWeapon))
		{
			return FText::GetEmpty();
		}

		const float Current =
			MageWeapon->GetExplosionRadius();

		const float Amount =
			MageWeapon->GetExplosionRadiusUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("폭발 범위 %.0f → %.0f (+%.0f)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::MageReflectDuration:
	{
		const ASlimeMage* Mage =
			Cast<ASlimeMage>(this);

		if (!IsValid(Mage))
		{
			return FText::GetEmpty();
		}

		const float Current =
			Mage->GetReflectDuration();

		const float Amount =
			Mage->GetReflectDurationUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("반사 지속시간 %.1f초 → %.1f초 (+%.1f초)"),
				Current,
				Current + Amount,
				Amount
			)
		);
	}

	case EPlayerUpgradeType::MageReflectDamage:
	{
		const ASlimeMage* Mage =
			Cast<ASlimeMage>(this);

		if (!IsValid(Mage))
		{
			return FText::GetEmpty();
		}

		const float Current =
			Mage->GetReflectDamageMultiplier();

		const float Amount =
			Mage->GetReflectDamageMultiplierUpgradeAmount();

		return FText::FromString(
			FString::Printf(
				TEXT("반사 피해 %.0f%% → %.0f%% (+%.0f%%)"),
				Current * 100.f,
				(Current + Amount) * 100.f,
				Amount * 100.f
			)
		);
	}

	default:
		return FText::GetEmpty();
	}
}

FString ASlimeCharacter::GetEarlyClearRewardDescription(
	EEarlyClearRewardType RewardType
) const
{
	switch (RewardType)
	{
	case EEarlyClearRewardType::Damage:
	{
		if (!IsValid(EquippedWeapon))
		{
			return TEXT("Damage UP");
		}

		const float Current =
			EquippedWeapon->GetDamage();

		const float Amount =
			EquippedWeapon->GetDamageUpgradeAmount();

		return FString::Printf(
			TEXT("Damage\n%.0f → %.0f (+%.0f)"),
			Current,
			Current + Amount,
			Amount
		);
	}

	case EEarlyClearRewardType::AttackSpeed:
	{
		if (!IsValid(EquippedWeapon))
		{
			return TEXT("Attack Speed UP");
		}

		const float Current =
			EquippedWeapon->GetAttackInterval();

		const float Amount =
			EquippedWeapon->GetAttackIntervalUpgradeAmount();

		const float Result =
			FMath::Max(
				0.1f,
				Current - Amount
			);

		return FString::Printf(
			TEXT("Attack Speed\n%.1f → %.1f (-%.1f)"),
			Current,
			Result,
			Current - Result
		);
	}

	case EEarlyClearRewardType::AttackRange:
	{
		if (!IsValid(EquippedWeapon))
		{
			return TEXT("Attack Range UP");
		}

		const float Current =
			EquippedWeapon->GetAttackRange();

		const float Amount =
			EquippedWeapon->GetAttackRangeUpgradeAmount();

		return FString::Printf(
			TEXT("Attack Range\n%.0f → %.0f (+%.0f)"),
			Current,
			Current + Amount,
			Amount
		);
	}

	case EEarlyClearRewardType::MaxHealth:
	{
		const float Current = MaxHealth;
		const float Amount = 20.f;

		return FString::Printf(
			TEXT("Max Health\n%.0f → %.0f (+%.0f)"),
			Current,
			Current + Amount,
			Amount
		);
	}

	case EEarlyClearRewardType::MoveSpeed:
	{
		const float Current =
			GetCharacterMovement()->MaxWalkSpeed;

		const float Amount = 30.f;

		return FString::Printf(
			TEXT("Move Speed\n%.0f → %.0f (+%.0f)"),
			Current,
			Current + Amount,
			Amount
		);
	}

	case EEarlyClearRewardType::Exp:
		return TEXT("EXP +50");

	default:
		return TEXT("");
	}
}

void ASlimeCharacter::ApplyUpgrade(EPlayerUpgradeType UpgradeType)
{
	switch (UpgradeType)
	{
	case EPlayerUpgradeType::Damage:
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeDamage();
		}
		break;
	}

	case EPlayerUpgradeType::AttackSpeed:
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeAttackSpeed();
		}
		break;
	}

	case EPlayerUpgradeType::AttackRange:
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeAttackRange();
		}
		break;
	}

	case EPlayerUpgradeType::MaxHealth:
	{
		MaxHealth += MaxHealthUpgradeAmount;

		// 최대 체력이 늘어난 만큼 현재 체력도 증가
		CurrentHealth += MaxHealthUpgradeAmount;

		UpdatePlayerHUD();
		break;
	}

	case EPlayerUpgradeType::MoveSpeed:
	{
		GetCharacterMovement()->MaxWalkSpeed +=
			MoveSpeedUpgradeAmount;

		break;
	}

	case EPlayerUpgradeType::WarriorMaxHitEnemies:
	{
		ASlimeWarriorWeapon* WarriorWeapon =
			Cast<ASlimeWarriorWeapon>(EquippedWeapon);

		if (IsValid(WarriorWeapon))
		{
			WarriorWeapon->UpgradeMaxHitEnemies();
		}

		break;
	}

	case EPlayerUpgradeType::WarriorAttackAngle:
	{
		ASlimeWarriorWeapon* WarriorWeapon =
			Cast<ASlimeWarriorWeapon>(EquippedWeapon);

		if (IsValid(WarriorWeapon))
		{
			WarriorWeapon->UpgradeAttackAngle();
		}

		break;
	}

	case EPlayerUpgradeType::ArcherProjectileCount:
	{
		if (IsValid(EquippedWeapon))
		{
			EquippedWeapon->UpgradeProjectileCount();
		}

		break;
	}

	case EPlayerUpgradeType::ArcherDashCooldown:
	{
		ASlimeArcher* Archer =
			Cast<ASlimeArcher>(this);

		if (IsValid(Archer))
		{
			Archer->UpgradeDashCooldown();
		}

		break;
	}

	case EPlayerUpgradeType::MageExplosionRadius:
	{
		ASlimeMageWeapon* MageWeapon =
			Cast<ASlimeMageWeapon>(EquippedWeapon);

		if (IsValid(MageWeapon))
		{
			MageWeapon->UpgradeExplosionRadius();
		}

		break;
	}

	case EPlayerUpgradeType::MageReflectDuration:
	{
		ASlimeMage* Mage =
			Cast<ASlimeMage>(this);

		if (IsValid(Mage))
		{
			Mage->UpgradeReflectDuration();
		}

		break;
	}

	case EPlayerUpgradeType::MageReflectDamage:
	{
		ASlimeMage* Mage =
			Cast<ASlimeMage>(this);

		if (IsValid(Mage))
		{
			Mage->UpgradeReflectDamage();
		}

		break;
	}
	}

	// 업그레이드 적용 후 HUD 갱신
	UpdatePlayerHUD();
}