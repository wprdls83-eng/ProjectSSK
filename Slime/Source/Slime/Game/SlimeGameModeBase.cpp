// SlimeGameModeBase.cpp 

#include "Slime/Game/SlimeGameModeBase.h"
#include "Slime/Character/SlimeCharacter.h" // 기본 플레이어 캐릭터로 사용할 클래스

ASlimeGameModeBase::ASlimeGameModeBase()
{
	// 게임 시작 시 생성될 기본 캐릭터 클래스를 지정
	DefaultPawnClass = ASlimeCharacter::StaticClass();
}
