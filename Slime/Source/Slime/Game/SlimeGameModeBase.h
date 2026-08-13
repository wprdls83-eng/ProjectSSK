// SlimeGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SlimeGameModeBase.generated.h"

class ASlimeCharacter;

UCLASS()
class SLIME_API ASlimeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASlimeGameModeBase();
	
protected:
	// 전사 Blueprint Character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Class")
	TSubclassOf<ASlimeCharacter> WarriorClass;

	// 궁수 Blueprint Character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Class")
	TSubclassOf<ASlimeCharacter> ArcherClass;

	// 마법사 Blueprint Character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Class")
	TSubclassOf<ASlimeCharacter> MageClass;

protected:
	virtual void BeginPlay() override;

	// 직업 선택 화면에서 선택한 캐릭터 생성
	void SpawnSelectedCharacter();
};
