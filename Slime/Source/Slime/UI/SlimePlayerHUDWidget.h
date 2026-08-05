// SlimePlayerHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlimePlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class SLIME_API USlimePlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // HP, EXP, Level 값을 한 번에 갱신
    void UpdateHUD(
        float CurrentHealth,
        float MaxHealth,
        int32 CurrentExp,
        int32 NeedExp,
        int32 PlayerLevel
    );

protected:
    // WBP_PlayerHUD의 HPBar와 자동 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HPBar;

    // WBP_PlayerHUD의 EXPBar와 자동 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> EXPBar;

    // WBP_PlayerHUD의 LevelText와 자동 연결
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> LevelText;
};
