#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Components/TimelineComponent.h"
#include "GC_HitStop.generated.h"

UCLASS()
class PROJECTNL_API AGC_HitStop : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AGC_HitStop();

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTimelineComponent> HitStopTimeline;
	
	UPROPERTY(EditAnywhere, Category="Options", meta = (AllowPrivateAccess = true))
	TObjectPtr<UCurveFloat> HitStopTimingCurve;
	
	UPROPERTY(EditAnywhere, Category="Options", meta = (AllowPrivateAccess = true))
	float HitStopRadius = 1000.f;
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	
	UPROPERTY()
	TObjectPtr<AActor> SourceActor;
	
	TArray<FOverlapResult> OverlapResults;
	
	FOnTimelineFloat HitStopCallback;
	
	FOnTimelineEvent HitStopFinish;

	UFUNCTION()
	void OnHitStopPlayCallback(float Output);
	
	UFUNCTION()
	void OnHitStopFinish();
};
