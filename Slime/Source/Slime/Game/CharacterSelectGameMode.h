// CharacterSelectGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Slime/Data/SlimeGameTypes.h"
#include "CharacterSelectGameMode.generated.h"

class ASlimeCharacter;

UCLASS()
class SLIME_API ACharacterSelectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACharacterSelectGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// UI에서 직업을 선택했을 때 호출
	UFUNCTION(BlueprintCallable, Category = "Character Select")
	void SelectCharacter(ESlimePlayerClass SelectedClass);

};
