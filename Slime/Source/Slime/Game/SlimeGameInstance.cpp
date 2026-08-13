// SlimeGameInstance.cpp

#include "Slime/Game/SlimeGameInstance.h"

void USlimeGameInstance::SetSelectedPlayerClass(
	ESlimePlayerClass NewPlayerClass
)
{
	// 선택한 직업 저장
	SelectedPlayerClass = NewPlayerClass;
}

ESlimePlayerClass USlimeGameInstance::GetSelectedPlayerClass() const
{
	// 현재 선택된 직업 반환
	return SelectedPlayerClass;
}

