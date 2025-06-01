// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/Character/Enemy/EnemyCharacter.h"
#include "BossEnemyCharacter.generated.h"


class UEnemyStatus;
UCLASS()
class PROJECTNL_API ABossEnemyCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ABossEnemyCharacter();

	virtual void Tick(float DeltaSeconds) override;

protected:
	// 화면에 띄울 위젯 블루프린트 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEnemyStatus> EnemyStatusWidgetClass;

private:
	// 생성된 위젯 인스턴스
	UPROPERTY()
	UEnemyStatus* EnemyStatusWidget = nullptr;

	// 플레이어와 위젯 가시거리
	static constexpr float VisibilityDistance = 3000.f;
};
