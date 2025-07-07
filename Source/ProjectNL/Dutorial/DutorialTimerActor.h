// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DutorialTimerActor.generated.h"

class UUserWidget;
class UBoxComponent;          // 트리거용

UCLASS()
class PROJECTNL_API ADutorialTimerActor : public AActor
{
	GENERATED_BODY()

public:
	ADutorialTimerActor();

protected:
	virtual void BeginPlay() override;

	/** 플레이어가 트리거에 들어오면 호출 */
	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor*              OtherActor,
		UPrimitiveComponent* OtherComp,
		int32                OtherBodyIndex,
		bool                 bFromSweep,
		const FHitResult&    SweepResult);

private:
	/* ───── Components ───── */
	/** 충돌 감지를 위한 Box (BP에서 사이즈 조정 가능) */
	UPROPERTY(VisibleAnywhere, Category = "Tutorial")
	TObjectPtr<UBoxComponent> TriggerBox;

	/* ───── UI & Timer ───── */
	UPROPERTY(EditDefaultsOnly, Category = "Tutorial")
	TSubclassOf<UUserWidget> TutorialWidgetClass;

	UPROPERTY()
	UUserWidget* SpawnedWidget = nullptr;

	FTimerHandle HideTimerHandle;

	void ShowWidget();
	void HideWidget();
};