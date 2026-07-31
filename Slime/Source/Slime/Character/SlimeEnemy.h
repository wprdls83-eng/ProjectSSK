// SlimeEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SlimeEnemy.generated.h"

UCLASS()
class SLIME_API ASlimeEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ASlimeEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
