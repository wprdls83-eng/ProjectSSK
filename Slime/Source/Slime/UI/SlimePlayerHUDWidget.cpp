// SlimePlayerHUDWidget.cpp

#include "Slime/UI/SlimePlayerHUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlimePlayerHUDWidget::UpdateHUD(
    float CurrentHealth,
    float MaxHealth,
    int32 CurrentExp,
    int32 NeedExp,
    int32 PlayerLevel
)
{
    // 최대 체력이 0보다 클 때만 HP 비율 계산
    if (IsValid(HPBar) && MaxHealth > 0.f)
    {
        const float HealthPercent =
            CurrentHealth / MaxHealth;

        HPBar->SetPercent(HealthPercent);
    }

    // 필요 경험치가 0보다 클 때만 EXP 비율 계산
    if (IsValid(EXPBar) && NeedExp > 0)
    {
        const float ExpPercent =
            static_cast<float>(CurrentExp) /
            static_cast<float>(NeedExp);

        EXPBar->SetPercent(ExpPercent);
    }

    // 현재 레벨 표시
    if (IsValid(LevelText))
    {
        LevelText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("LV %d"),
                    PlayerLevel
                )
            )
        );
    }
}