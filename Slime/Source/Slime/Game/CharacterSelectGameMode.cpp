// CharacterSelectGameMode.cpp

#include "Slime/Game/CharacterSelectGameMode.h"
#include "Slime/Game/SlimeGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

ACharacterSelectGameMode::ACharacterSelectGameMode()
{
	// 직업 선택 맵에서는 플레이어 캐릭터가 필요하지 않음
	DefaultPawnClass = nullptr;
}

void ACharacterSelectGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 첫 번째 PlayerController 가져오기
	APlayerController* PlayerController =
		UGameplayStatics::GetPlayerController(
			this,
			0
		);

	if (!IsValid(PlayerController))
	{
		return;
	}

	// Level에 배치된 CameraActor 찾기
	AActor* CameraActor =
		UGameplayStatics::GetActorOfClass(
			this,
			ACameraActor::StaticClass()
		);

	if (!IsValid(CameraActor))
	{
		return;
	}

	// 직업 선택용 카메라로 화면 전환
	PlayerController->SetViewTargetWithBlend(
		CameraActor,
		0.f
	);
}

void ACharacterSelectGameMode::SelectCharacter(
	ESlimePlayerClass SelectedClass
)
{
	// 현재 GameInstance 가져오기
	USlimeGameInstance* SlimeGameInstance =
		Cast<USlimeGameInstance>(GetGameInstance());

	if (!IsValid(SlimeGameInstance))
	{
		return;
	}

	// 선택한 직업을 GameInstance에 저장
	SlimeGameInstance->SetSelectedPlayerClass(
		SelectedClass
	);

	// 실제 게임을 진행할 Level로 이동
	UGameplayStatics::OpenLevel(
		this,
		FName(TEXT("Test"))
	);
}