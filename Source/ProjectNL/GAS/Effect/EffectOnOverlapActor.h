#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "EffectOnOverlapActor.generated.h"

class UBoxComponent;

/**
 *  박스 충돌 시 지정한 GameplayEffect를 상대에게 적용합니다.
 */
UCLASS()
class PROJECTNL_API AEffectOnOverlapActor : public AActor
{
	GENERATED_BODY()

public:
	AEffectOnOverlapActor();

protected:
	// 충돌 판정을 위한 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	UBoxComponent* CollisionBox;

	// 오버랩된 액터에게 적용할 이펙트 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TSubclassOf<UGameplayEffect> EffectToApplyClass;

	// 충돌 시작 시 호출되는 함수
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
