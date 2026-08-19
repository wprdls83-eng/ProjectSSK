// SlimeCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Slime/Data/SlimeDataTypes.h"
#include "SlimeCharacter.generated.h"

// 전방 선언
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ASlimeWeaponBase;
class USlimePlayerHUDWidget;
class USlimeGameOverWidget;
struct FInputActionValue;

UCLASS()
class SLIME_API ASlimeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlimeCharacter();

	// 선택한 업그레이드를 실제로 적용
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	void ApplyUpgrade(EPlayerUpgradeType UpgradeType);

protected:
	// 플레이어와 카메라 사이의 거리를 유지하는 스프링암
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	// 실제 화면을 출력하는 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComp;

	// 플레이어가 사용할 입력 매핑 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	// 이동 입력 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 직업 고유 능력 입력 액션
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SpecialAbilityAction;

	// 플레이어가 생성할 무기 Blueprint 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASlimeWeaponBase> WeaponClass;

	// 현재 플레이어가 장착 중인 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ASlimeWeaponBase> EquippedWeapon;

	// 레벨업 UI Blueprint 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LevelUpWidgetClass;

	// 생성할 Player HUD 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USlimePlayerHUDWidget> PlayerHUDWidgetClass;

	// 생성된 레벨업 UI
	UPROPERTY()
	TObjectPtr<UUserWidget> LevelUpWidget;

	// 게임 중 생성된 Player HUD 위젯
	UPROPERTY()
	TObjectPtr<USlimePlayerHUDWidget> PlayerHUDWidget;

	// 생성할 Game Over 위젯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USlimeGameOverWidget> GameOverWidgetClass;

	// 생성된 Game Over 위젯
	UPROPERTY()
	TObjectPtr<USlimeGameOverWidget> GameOverWidget;

	// 이번 레벨업에서 UI에 표시할 업그레이드 3개
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Upgrade")
	TArray<EPlayerUpgradeType> CurrentUpgradeChoices;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	float MaxHealth = 100.f;

	// 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	float CurrentHealth;

	// 자동 조준할 Enemy 탐색 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Target")
	float TargetDetectRange = 800.f;

	// 최대 체력 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Upgrade")
	float MaxHealthUpgradeAmount = 20.f;

	// 이동 속도 업그레이드 증가량
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player|Upgrade")
	float MoveSpeedUpgradeAmount = 30.f;

	// 직업 선택 화면에서 사용하는 Preview 캐릭터인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Preview")
	bool bIsPreviewCharacter = false;

	// 현재 플레이어가 사망했는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Health")
	bool bIsDead = false;

	// 현재 플레이어 경험치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Exp")
	int32 CurrentExp = 0;

	// 현재 플레이어 레벨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Level")
	int32 PlayerLevel = 1;

	// 다음 레벨까지 필요한 경험치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Level")
	int32 NeedExp = 15;

	// 아직 업그레이드를 선택하지 않은 레벨업 횟수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Level", meta = (AllowPrivateAccess = "true"))
	int32 PendingLevelUps;

	// 현재 플레이어가 입력하고 있는 이동 방향
	FVector CurrentMoveDirection = FVector::ZeroVector;

protected:
	// WASD 입력을 받아 캐릭터를 이동시키는 함수
	void Move(const FInputActionValue& Value);

	// 게임 시작 함수
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// 입력 액션과 함수를 연결하는 함수
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 직업 고유 능력 사용
	virtual void UseSpecialAbility();

	// 현재 Enemy 데미지를 받을 수 있는지 확인
	virtual bool CanTakeDamageFromEnemy() const;

	// 플레이어 사망 처리
	void Die();

	// Game Over UI 표시
	void ShowGameOverUI();

	// 가장 가까운 Enemy를 계속 바라봄
	void LookAtNearestEnemy();

public:
	// 경험치 받는 함수
	void AddExp(int32 ExpAmount);
	
	// 레밸업 함수
	void LevelUp();

	// 레벨업 UI를 화면에 표시
	void ShowLevelUpUI();

	// HUD 갱신
	void UpdatePlayerHUD();

	// 무기 데미지 증가
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	void UpgradeDamage();

	// 공격 속도 증가
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	void UpgradeAttackSpeed();

	// 한 번에 발사하는 Projectile 개수 증가
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	void UpgradeProjectileCount();

	// 업그레이드 선택 완료 후 남은 레벨업 횟수 처리
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	void CompleteLevelUpSelection();

	// 플레이어 피격 처리
	UFUNCTION(BlueprintCallable, Category = "Player|Health")
	void TakeDamageFromEnemy(float DamageAmount);

	// Early Clear 보상 적용
	void ApplyEarlyClearReward(EEarlyClearRewardType RewardType);

	// 현재 직업의 업그레이드 목록에서 중복 없이 랜덤 3개 생성
	void GenerateUpgradeChoices();

	// GameMode에서 Possess가 끝난 후 플레이어 초기화
	void InitializePlayer();

	// 현재 최대 체력 반환
	float GetMaxHealth() const { return MaxHealth; }

	// 최대 체력 증가량 반환
	float GetMaxHealthUpgradeAmount() const { return MaxHealthUpgradeAmount; }

	// 현재 이동 속도 반환
	float GetMoveSpeed() const;

	// 이동 속도 증가량 반환
	float GetMoveSpeedUpgradeAmount() const { return MoveSpeedUpgradeAmount; }

	// 현재 직업에서 등장할 수 있는 업그레이드 목록 반환
	virtual TArray<EPlayerUpgradeType> GetAvailableUpgrades() const;

	// 현재 생성된 업그레이드 선택지 반환
	UFUNCTION(BlueprintCallable, Category = "Upgrade")
	TArray<EPlayerUpgradeType> GetCurrentUpgradeChoices() const;

	// 업그레이드 종류에 맞는 표시 이름 반환
	UFUNCTION(BlueprintPure, Category = "Upgrade")
	FText GetUpgradeDisplayName(EPlayerUpgradeType UpgradeType) const;

	// 업그레이드 종류에 맞는 설명 반환
	UFUNCTION(BlueprintPure, Category = "Upgrade")
	FText GetUpgradeDescription(EPlayerUpgradeType UpgradeType) const;

	// 조기 클리어 보상의 변경 수치를 UI용 문자열로 반환
	FString GetEarlyClearRewardDescription(
		EEarlyClearRewardType RewardType
	) const;
};
