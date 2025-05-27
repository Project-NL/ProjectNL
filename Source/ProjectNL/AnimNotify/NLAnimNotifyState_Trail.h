// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState_Trail.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Weapon/BaseWeapon.h"
#include "NLAnimNotifyState_Trail.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API UNLAnimNotifyState_Trail : public UAnimNotifyState_Trail
{
	    GENERATED_BODY()

protected:

    // NotifyBegin: 트레일 파티클 생성
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
                             UAnimSequenceBase* Animation,
                             float TotalDuration,
                             const FAnimNotifyEventReference& EventReference) override
    {
        // 1) 무기 MeshComp 를 찾아온다
        USkeletalMeshComponent* WeaponMesh = GetWeaponMeshComponent(MeshComp);
        if (!WeaponMesh || !ValidateInput(WeaponMesh, /*bReportErrors=*/true))
        {
            return;
        }

        // 2) 부모 로직을 호출하되, MeshComp 를 WeaponMesh 로 바꿔서 전달
        Super::NotifyBegin(WeaponMesh, Animation, TotalDuration, EventReference);
    }

    // NotifyTick: 매 틱마다 위치 업데이트
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp,
                            UAnimSequenceBase* Animation,
                            float FrameDeltaTime,
                            const FAnimNotifyEventReference& EventReference) override
    {
        USkeletalMeshComponent* WeaponMesh = GetWeaponMeshComponent(MeshComp);
        if (!WeaponMesh) return;

        Super::NotifyTick(WeaponMesh, Animation, FrameDeltaTime, EventReference);
    }

    // NotifyEnd: 트레일 종료
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,
                           UAnimSequenceBase* Animation,
                           const FAnimNotifyEventReference& EventReference) override
    {
        USkeletalMeshComponent* WeaponMesh = GetWeaponMeshComponent(MeshComp);
        if (!WeaponMesh) return;

        Super::NotifyEnd(WeaponMesh, Animation, EventReference);
    }


private:
    /// 캐릭터→EquipComponent→장착된 무기 액터→SkeletalMeshComponent 를 찾아 리턴
    USkeletalMeshComponent* GetWeaponMeshComponent(USkeletalMeshComponent* MeshComp) const
    {
        if (ABaseCharacter* OwnerChar = Cast<ABaseCharacter>(MeshComp->GetOwner()))
        {
            if (auto* EquipComp = OwnerChar->GetEquipComponent())
            {
                if (ABaseWeapon* WeaponActor = EquipComp->GetMainWeapon())  // 사용자 구현 함수
                {
                    return WeaponActor->FindComponentByClass<USkeletalMeshComponent>();
                }
            }
        }
        return nullptr;
    }
};
