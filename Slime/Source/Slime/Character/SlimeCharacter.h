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

	// WASD 입력을 받아 캐릭터를 이동시키는 함수
	void Move(const FInputActionValue& Value);

	// 게임 시작 함수
	virtual void BeginPlay() override;

	// 입력 액션과 함수를 연결하는 함수
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

};
