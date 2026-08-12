// SlimeGameOverWidget.cpp

#include "Slime/UI/SlimeGameOverWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void USlimeGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// RestartButton이 정상적으로 연결되어 있다면
	if (IsValid(RestartButton))
	{
		// 버튼 클릭 시 재시작 함수 호출
		RestartButton->OnClicked.AddDynamic(
			this,
			&USlimeGameOverWidget::OnRestartButtonClicked
		);
	}
}

void USlimeGameOverWidget::OnRestartButtonClicked()
{
	// 현재 PlayerController 가져오기
	APlayerController* PlayerController =
		GetOwningPlayer();

	if (IsValid(PlayerController))
	{
		// 마우스 커서 숨기기
		PlayerController->bShowMouseCursor = false;

		// 입력을 다시 게임 전용으로 변경
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}

	// 게임 일시정지 해제
	UGameplayStatics::SetGamePaused(
		this,
		false
	);

	// 현재 Level 이름 가져오기
	const FString CurrentLevelName =
		UGameplayStatics::GetCurrentLevelName(
			this,
			true
		);

	// 현재 Level 다시 시작
	UGameplayStatics::OpenLevel(
		this,
		FName(*CurrentLevelName)
	);
}