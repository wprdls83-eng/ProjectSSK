// SlimeDamageTextWidget.cpp

#include "SlimeDamageTextWidget.h"

#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void USlimeDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Damage Text 애니메이션이 끝나면
	// Widget을 Viewport에서 자동 제거
	if (IsValid(DamageTextAnim))
	{
		FWidgetAnimationDynamicEvent AnimationFinishedEvent;

		AnimationFinishedEvent.BindDynamic(
			this,
			&USlimeDamageTextWidget::RemoveDamageText
		);

		BindToAnimationFinished(
			DamageTextAnim,
			AnimationFinishedEvent
		);
	}
}

void USlimeDamageTextWidget::SetDamage(float DamageAmount)
{
	// DamageText가 정상적으로 연결되어 있는지 확인
	if (!IsValid(DamageText))
	{
		return;
	}

	// 실제로 받은 데미지를 정수 형태의 Text로 변환
	const int32 DisplayDamage =
		FMath::RoundToInt(DamageAmount);

	// WBP_DamageText의 Text에 데미지 숫자 표시
	DamageText->SetText(
		FText::AsNumber(DisplayDamage)
	);

	// Floating Text 애니메이션 재생
	if (IsValid(DamageTextAnim))
	{
		PlayAnimation(DamageTextAnim);
	}
}

void USlimeDamageTextWidget::RemoveDamageText()
{
	RemoveFromParent();
}

