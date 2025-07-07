#include "ValuePercentBar.h"
#include "Components/ProgressBar.h"

void UValuePercentBar::OnWidgetRebuilt()
{
    Super::OnWidgetRebuilt();

    if (!ensureMsgf(IsValid(ViewPercentBar) && IsValid(DelayViewPercentBar),
        TEXT("[ValuePercentBar] ProgressBar not bound!")))
    {
        return;
    }

    ViewPercentBar->SetFillColorAndOpacity(MainColor);
    DelayViewPercentBar->SetFillColorAndOpacity(SubColor);
}

void UValuePercentBar::InitializePercent(float CurrentValue, float MaxValue)
{
    if (!ensure(MaxValue > 0.f))
    {
        PercentNum = { 0.f, 1.f };
        return;
    }

    PercentNum = { CurrentValue, MaxValue };
    const float Percent = SafeDiv(CurrentValue, MaxValue);

    if (IsValid(ViewPercentBar)) ViewPercentBar->SetPercent(Percent);
    if (IsValid(DelayViewPercentBar)) DelayViewPercentBar->SetPercent(Percent);
}

void UValuePercentBar::SetCurrentValue(float NewValue)
{
    if (!IsValid(ViewPercentBar) || !IsValid(DelayViewPercentBar)) return;

    // 감소하는 경우 지연 애니메이션 사용
    if (NewValue < PercentNum.Key)
    {
        const float PrevCurrent = PercentNum.Key;
        PercentNum.Key = NewValue;

        const float PrevPercent = SafeDiv(PrevCurrent, PercentNum.Value);
        const float NewPercent  = SafeDiv(NewValue,  PercentNum.Value);

        ViewPercentBar->SetPercent(NewPercent);

        DelayPercentInfo = { PrevPercent, NewPercent };
        DecreaseDelayPercentBar();
    }
    else // 증가 or 동일 → 즉시 반영
    {
        PercentNum.Key = NewValue;
        const float NewPercent = SafeDiv(NewValue, PercentNum.Value);

        ViewPercentBar->SetPercent(NewPercent);
        DelayViewPercentBar->SetPercent(NewPercent);
    }
}

void UValuePercentBar::SetMaxValue(float NewValue)
{
    if (!IsValid(ViewPercentBar) || !IsValid(DelayViewPercentBar)) return;

    PercentNum.Value = FMath::Max(1.f, NewValue);  // 0 방어
    const float NewPercent = SafeDiv(PercentNum.Key, PercentNum.Value);

    DelayPercentInfo.FinalPercent = NewPercent;

    ViewPercentBar->SetPercent(NewPercent);
    DelayViewPercentBar->SetPercent(NewPercent);
}

void UValuePercentBar::DecreaseDelayPercentBar()
{
    UWorld* World = GetWorld();
    if (!IsValid(World) || !IsValid(DelayViewPercentBar)) return;

    // 중복 실행 방지
    World->GetTimerManager().ClearTimer(SetDelayViewPercentTimerHandle);

    TWeakObjectPtr<UValuePercentBar> WeakSelf(this);

    FTimerDelegate DecreaseDel;
    DecreaseDel.BindLambda([WeakSelf]()
    {
        if (!WeakSelf.IsValid()) return;

        UValuePercentBar* Self = WeakSelf.Get();
        if (!IsValid(Self->DelayViewPercentBar)) return;

        // 0.01 씩 감소 (튜닝 가능)
        Self->DelayPercentInfo.CurrentPercent =
            FMath::Max(Self->DelayPercentInfo.CurrentPercent - 0.01f,
                       Self->DelayPercentInfo.FinalPercent);

        Self->DelayViewPercentBar->SetPercent(Self->DelayPercentInfo.CurrentPercent);

        // 목표치에 도달하면 타이머 종료
        if (Self->DelayPercentInfo.CurrentPercent <= Self->DelayPercentInfo.FinalPercent)
        {
            if (UWorld* W = Self->GetWorld())
            {
                W->GetTimerManager().ClearTimer(Self->SetDelayViewPercentTimerHandle);
            }
        }
    });

    World->GetTimerManager().SetTimer(
        SetDelayViewPercentTimerHandle,
        DecreaseDel,
        PercentDecreaseTime,
        true,
        DelayedTime);
}

void UValuePercentBar::NativeDestruct()
{
    Super::NativeDestruct();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SetDelayViewPercentTimerHandle);
    }
}