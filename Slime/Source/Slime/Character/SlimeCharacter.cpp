// SlimeCharacter.cpp

#include "Slime/Character/SlimeCharacter.h"
#include "Slime/Weapons/SlimeWeaponBase.h"

#include "Camera/CameraComponent.h" // 카메라 컴포넌트
#include "GameFramework/SpringArmComponent.h" // 스프링암 컴포넌트
#include "GameFramework/CharacterMovementComponent.h" // CharacterMovement를 사용하기 위한 헤더

// Enhanced Input 관련 헤더
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

ASlimeCharacter::ASlimeCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// 컨트롤러 회전을 사용하지 않음
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 이동하는 방향을 바라보도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 회전 속도
	GetCharacterMovement()->RotationRate = FRotator(0.f, 50.f, 0.f);

	// SpringArm 컴포넌트 생성
	SpringArmComp =	CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));

	// 루트(Capsule)에 부착
	SpringArmComp->SetupAttachment(RootComponent);

	// 카메라와 캐릭터 사이 거리
	SpringArmComp->TargetArmLength = 900.f;

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

void ASlimeCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 현재 캐릭터를 조종하는 PlayerController를 가져온다.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (IsValid(PlayerController) == false)
	{
		return;
	}

	// PlayerController가 가진 LocalPlayer를 가져온다.
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

	if (IsValid(LocalPlayer) == false)
	{
		return;
	}

	// LocalPlayer에서 Enhanced Input Subsystem을 가져온다.
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (IsValid(InputSubsystem) == false)
	{
		return;
	}

	// 블루프린트에서 지정한 IMC_Player를 등록한다.
	if (PlayerMappingContext)
	{
		InputSubsystem->AddMappingContext(PlayerMappingContext, 0);
	}

	// 무기 클래스 설정 실패했을 때
	if (!WeaponClass)
	{
		return;
	}

	// 생성할 무기의 Owner와 Instigator를 플레이어로 설정
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;

	// 플레이어의 현재 위치와 회전에 무기 생성
	EquippedWeapon = GetWorld()->SpawnActor<ASlimeWeaponBase>(
		WeaponClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParameters
	);

	// 무기 생성에 실패했다면 이후 코드 실행 중지
	if (!IsValid(EquippedWeapon))
	{
		return;
	}

	// 생성된 무기를 플레이어의 루트 컴포넌트에 부착
	EquippedWeapon->AttachToComponent(
		GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);

	// 무기가 플레이어 몸 중앙에 겹치지 않도록 상대 위치 조정
	EquippedWeapon->SetActorRelativeLocation(
		FVector(0.f, 0.f, 50.f)
	);
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
}

void ASlimeCharacter::Move(const FInputActionValue& Value)
{
	// IA_Move는 Axis2D이므로 FVector2D로 값을 받는다.
	const FVector2D MoveInput = Value.Get<FVector2D>();

	// X값은 좌우 이동
	if (FMath::IsNearlyZero(MoveInput.X) == false)
	{
		AddMovementInput(GetActorRightVector(), MoveInput.X);
	}

	// Y값은 앞뒤 이동
	if (FMath::IsNearlyZero(MoveInput.Y) == false)
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.Y);
	}
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

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("경험치 획득: +%d | 현재 경험치: %d / %d"),
		ExpAmount,
		CurrentExp,
		NeedExp
	);

	// 필요한 경험치 이상이면 레벨업
	while (CurrentExp >= NeedExp)
	{
		LevelUp();
	}
}

void ASlimeCharacter::LevelUp()
{
	// 레벨업에 사용한 경험치 차감
	CurrentExp -= NeedExp;

	// 플레이어 레벨 증가
	PlayerLevel++;

	// 다음 레벨 필요 경험치 증가
	NeedExp = FMath::RoundToInt(NeedExp * 1.5f);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("LEVEL UP! | 현재 레벨: %d | 남은 경험치: %d | 다음 필요 경험치: %d"),
		PlayerLevel,
		CurrentExp,
		NeedExp
	);
}

