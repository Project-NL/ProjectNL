// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WeaponSoundAnimNotify.generated.h"

class USoundAttenuation;

/**
 * AnimNotify로 무기 휘두를 때 사운드를 재생하는 클래스.
 * 여기서 AttenuationSettings 프로퍼티를 통해 3D 감쇠를 지정할 수 있다.
 */
UCLASS(const, Blueprintable)
class PROJECTNL_API UWeaponSoundAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 애튜에뉴에이션 에셋을 에디터에서 지정할 수 있도록 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundAttenuation* AttenuationSettings;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
