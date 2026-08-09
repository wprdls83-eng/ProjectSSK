// SlimeSplit.cpp

#include "Slime/Enemy/SlimeSplit.h"

ASlimeSplit::ASlimeSplit()
{

}

void ASlimeSplit::Die()
{
	// 분열 가능한 상태라면 작은 Enemy 생성
	if (bCanSplit)
	{
		SpawnSplitEnemies();
	}

	// EXP 드랍 + Wave 처치 처리 + 현재 Enemy 제거
	FinishDeath();
}

void ASlimeSplit::SpawnSplitEnemies()
{
	// 생성할 Enemy 클래스가 설정되지 않았다면 종료
	if (!SplitEnemyClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("%s : SplitEnemyClass가 설정되지 않았습니다."),
			*GetName()
		);

		return;
	}

	// 설정된 개수만큼 작은 Enemy 생성
	for (int32 i = 0; i < SplitCount; ++i)
	{
		// 현재 분열형 Enemy의 위치
		FVector SpawnLocation = GetActorLocation();

		// 생성되는 Enemy끼리 겹치지 않도록 좌우로 위치 조정
		const float SideOffset =
			(i - (SplitCount - 1) * 0.5f) * 150.f;

		SpawnLocation += GetActorRightVector() * SideOffset;

		// 작은 Enemy 생성
		ASlimeEnemy* SplitEnemy =
			GetWorld()->SpawnActor<ASlimeEnemy>(
				SplitEnemyClass,
				SpawnLocation,
				GetActorRotation()
			);

		// 정상적으로 생성되었다면 크기 감소
		if (IsValid(SplitEnemy))
		{
			// 크기 감소
			SplitEnemy->SetActorScale3D(
				GetActorScale3D() * SplitScaleRatio
			);

			// 체력 감소
			SplitEnemy->MaxHealth =
				MaxHealth * SplitHealthRatio;

			SplitEnemy->CurrentHealth =
				SplitEnemy->MaxHealth;

			// 공격력 감소
			SplitEnemy->AttackDamage =
				AttackDamage * SplitDamageRatio;

			// 분열형이라면 다시 분열하지 못하도록 설정
			ASlimeSplit* SplitSlime =
				Cast<ASlimeSplit>(SplitEnemy);

			if (IsValid(SplitSlime))
			{
				SplitSlime->bCanSplit = false;
			}
		}
	}
}