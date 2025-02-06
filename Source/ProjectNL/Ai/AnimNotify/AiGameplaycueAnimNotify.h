// #pragma once
//
// #include "CoreMinimal.h"
// #include "Animation/AnimNotifies/AnimNotify.h"
// #include "GameplayTagContainer.h" // For Gameplay Tags
// #include "AiGameplaycueAnimNotify.generated.h"
//
// /**
//  * GameplayCue를 트리거하는 AnimNotify
//  */
// UCLASS()
// class PROJECTNL_API UAiGameplaycueAnimNotify : public UAnimNotify
// {
// 	GENERATED_BODY()
//
// public:
// 	/** Notify 실행 시 호출 */
// 	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
//
// protected:
// 	/** 트리거할 GameplayCue의 Tag */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
// 	FGameplayTag GameplayCueTag;
//
// 	/** 트리거 시 추가로 전달할 매개변수 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
// 	FVector CueLocationOffset = FVector::ZeroVector;
//
// 	/** Debug용 로그 출력 여부 */
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayCue")
// 	bool bDebug = false;
// };
