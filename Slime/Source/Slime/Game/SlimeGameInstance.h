// SlimeGameInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Slime/Data/SlimeGameTypes.h"
#include "SlimeGameInstance.generated.h"

UCLASS()
class SLIME_API USlimeGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	// 직업 선택 화면에서 선택한 플레이어 직업
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	ESlimePlayerClass SelectedPlayerClass = ESlimePlayerClass::Warrior;

public:
	// 선택한 직업 저장
	void SetSelectedPlayerClass(ESlimePlayerClass NewPlayerClass);

	// 현재 선택한 직업 반환
	ESlimePlayerClass GetSelectedPlayerClass() const;
};
