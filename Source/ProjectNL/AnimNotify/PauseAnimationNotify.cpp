// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/AnimNotify/PauseAnimationNotify.h"

void UPauseAnimationNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
//	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (AnimInstance)
	{
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
        
		if (CurrentMontage && AnimInstance->Montage_IsPlaying(CurrentMontage))
		{
			UE_LOG(LogTemp, Warning, TEXT("현재 재생 중인 몽타주: %s"), *CurrentMontage->GetName());
			AnimInstance->Montage_Pause(CurrentMontage);
			UE_LOG(LogTemp, Warning, TEXT("몽타주 멈춤!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("현재 몽타주 없음 또는 재생 중 아님!"));
		}
	}
}
