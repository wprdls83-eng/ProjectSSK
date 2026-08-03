// SlimeCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlimeCharacter.generated.h"

// 전방 선언
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ASlimeWeaponBase;
struct FInputActionValue;

UCLASS()
class SLIME_API ASlimeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlimeCharacter();

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

	// 플레이어가 생성할 무기 Blueprint 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<ASlimeWeaponBase> WeaponClass;

	// 현재 플레이어가 장착 중인 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<ASlimeWeaponBase> EquippedWeapon;

	// 현재 플레이어 경험치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Exp")
	int32 CurrentExp = 0;

	// 현재 플레이어 레벨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Level")
	int32 PlayerLevel = 1;

	// 다음 레벨까지 필요한 경험치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Level")
	int32 NeedExp = 15;

protected:
	// WASD 입력을 받아 캐릭터를 이동시키는 함수
	void Move(const FInputActionValue& Value);

	// 게임 시작 함수
	virtual void BeginPlay() override;

	// 입력 액션과 함수를 연결하는 함수
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


public:
	// 경험치 받는 함수
	void AddExp(int32 ExpAmount);
	
	// 레밸업 함수
	void LevelUp();
};
