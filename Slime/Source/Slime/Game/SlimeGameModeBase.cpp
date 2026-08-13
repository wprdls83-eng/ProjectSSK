// SlimeGameModeBase.cpp 

#include "Slime/Game/SlimeGameModeBase.h"
#include "Slime/Game/SlimeGameInstance.h"
#include "Slime/Character/SlimeCharacter.h"
#include "Slime/Data/SlimeGameTypes.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ASlimeGameModeBase::ASlimeGameModeBase()
{
	// 직업 선택 이후 직접 캐릭터를 Spawn하므로
	// 기본 Pawn 자동 생성을 사용하지 않음
	DefaultPawnClass = nullptr;
}

void ASlimeGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 선택한 직업의 캐릭터 생성
	SpawnSelectedCharacter();
}

void ASlimeGameModeBase::SpawnSelectedCharacter()
{
	// GameInstance 가져오기
	USlimeGameInstance* SlimeGameInstance =
		Cast<USlimeGameInstance>(GetGameInstance());

	if (!IsValid(SlimeGameInstance))
	{
		return;
	}

	// 선택된 직업 확인
	const ESlimePlayerClass SelectedClass =
		SlimeGameInstance->GetSelectedPlayerClass();

	// 생성할 Character 클래스
	TSubclassOf<ASlimeCharacter> CharacterClass = nullptr;

	switch (SelectedClass)
	{
	case ESlimePlayerClass::Warrior:

		CharacterClass = WarriorClass;
		break;

	case ESlimePlayerClass::Archer:

		CharacterClass = ArcherClass;
		break;

	case ESlimePlayerClass::Mage:

		CharacterClass = MageClass;
		break;

	default:
		return;
	}

	// Blueprint에서 Character Class가 설정되지 않았다면 생성하지 않음
	if (!CharacterClass)
	{
		return;
	}

	// 현재 Level의 PlayerStart 찾기
	AActor* PlayerStart =
		UGameplayStatics::GetActorOfClass(
			this,
			APlayerStart::StaticClass()
		);

	if (!IsValid(PlayerStart))
	{
		return;
	}

	// PlayerStart 위치와 회전 가져오기
	const FVector SpawnLocation =
		PlayerStart->GetActorLocation();

	const FRotator SpawnRotation =
		PlayerStart->GetActorRotation();

	// 선택한 직업 Character 생성
	ASlimeCharacter* SpawnedCharacter =
		GetWorld()->SpawnActor<ASlimeCharacter>(
			CharacterClass,
			SpawnLocation,
			SpawnRotation
		);

	if (!IsValid(SpawnedCharacter))
	{
		return;
	}

	// 플레이어 Controller 가져오기
	APlayerController* PlayerController =
		UGameplayStatics::GetPlayerController(
			this,
			0
		);

	if (!IsValid(PlayerController))
	{
		SpawnedCharacter->Destroy();
		return;
	}

	// 생성한 Character를 플레이어가 조종하도록 설정
	PlayerController->Possess(
		SpawnedCharacter
	);

	// Controller가 연결된 이후 Input과 HUD 초기화
	SpawnedCharacter->InitializePlayer();

	// 게임 입력으로 변경
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

	// 직업 선택 화면에서 사용했던 마우스 커서 숨기기
	PlayerController->bShowMouseCursor = false;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("선택한 직업 Character Spawn 완료")
	);
}