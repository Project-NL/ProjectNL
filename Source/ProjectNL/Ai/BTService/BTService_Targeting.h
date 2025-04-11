// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_Targeting.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UBTService_Targeting : public UBTService_BlueprintBase
{
	GENERATED_BODY()
public:
	UBTService_Targeting();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 서비스가 비활성화되거나 더 이상 관련이 없어질 때 호출되는 함수
	
};
