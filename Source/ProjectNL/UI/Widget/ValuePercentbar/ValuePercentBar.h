#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValuePercentBar.generated.h"

class UOverlay;
class UProgressBar;

struct FDecreaseDelayPercentInfo
{
	float CurrentPercent;
	float FinalPercent;
};

// TODO: 추후 WBP 내부에서 색상 지정과 길이 조정 기능 등을 자유롭게 하는 기능
// 고도화 작업이 필요함.
UCLASS()
class PROJECTNL_API UValuePercentBar : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializePercent(const float CurrentValue, const float MaxValue);

	void SetCurrentValue(const float NewValue);

	void SetMaxValue(const float NewValue);
	
protected:
	virtual void OnWidgetRebuilt() override;
	virtual void NativeDestruct() override;
	// .h
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PercentBar")
	TObjectPtr<UProgressBar> ViewPercentBar = nullptr;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "PercentBar")
	TObjectPtr<UProgressBar> DelayViewPercentBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> PercentBarOverlay;

	FORCEINLINE float SafeDiv(float Numerator, float Denominator) const
	{
		return FMath::IsNearlyZero(Denominator) ? 0.f : Numerator / Denominator;
	}
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor MainColor;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FLinearColor SubColor;
	
	// N초 뒤 실행할 변수 값
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float DelayedTime = 1.2;

	// 줄어드는 시간 단위
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float PercentDecreaseTime = 0.025;
	
	FTimerHandle SetDelayViewPercentTimerHandle;

	FDecreaseDelayPercentInfo DelayPercentInfo;
	void DecreaseDelayPercentBar();


	// 0번째 현재 값, 1번째 최대 값
	TPair<float, float> PercentNum;
};
