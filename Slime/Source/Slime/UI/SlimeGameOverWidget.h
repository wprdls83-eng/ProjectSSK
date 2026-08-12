// SlimeGameOverWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlimeGameOverWidget.generated.h"

class UButton;

UCLASS()
class SLIME_API USlimeGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 다시 시작 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

protected:
	// Widget 생성 후 초기 설정
	virtual void NativeConstruct() override;

	// 다시 시작 버튼 클릭 처리
	UFUNCTION()
	void OnRestartButtonClicked();
};