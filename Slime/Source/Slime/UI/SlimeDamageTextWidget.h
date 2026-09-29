// SlimeDamageTextWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlimeDamageTextWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;
class USlimeDamageTextWidget;

UCLASS()
class SLIME_API USlimeDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// WBP_DamageText의 DamageText와 연결
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	// WBP_DamageText에서 만든 Floating Text 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DamageTextAnim;

	// Enemy 피격 시 생성할 데미지 Floating Text Widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|UI")
	TSubclassOf<USlimeDamageTextWidget> DamageTextWidgetClass;

public:
	virtual void NativeConstruct() override;

	// 실제로 Enemy가 받은 데미지를 화면에 표시
	void SetDamage(float DamageAmount);

	// Floating Text 애니메이션 종료 후 Widget 제거
	UFUNCTION()
	void RemoveDamageText();
};
