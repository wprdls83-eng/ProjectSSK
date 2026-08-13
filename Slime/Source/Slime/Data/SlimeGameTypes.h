// SlimeGameTypes.h

#pragma once

#include "CoreMinimal.h"
#include "SlimeGameTypes.generated.h"

// 플레이어가 선택할 수 있는 직업
UENUM(BlueprintType)
enum class ESlimePlayerClass : uint8
{
	Warrior UMETA(DisplayName = "전사"),
	Archer  UMETA(DisplayName = "궁수"),
	Mage    UMETA(DisplayName = "마법사")
};