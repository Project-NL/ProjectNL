// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/AnimNotify/WeaponSoundAnimNotify.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Weapon/BaseWeapon.h"

void UWeaponSoundAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		ABaseWeapon* Weapon = Owner->GetEquipComponent()->GetMainWeapon();
		if (Weapon)
		{
			USoundBase* SoundToPlay=	Weapon->GetSwingSound();
			if (SoundToPlay)
			{
				UGameplayStatics::SpawnSoundAtLocation(
					Weapon,
					SoundToPlay,
					Weapon->GetActorLocation(),
					FRotator::ZeroRotator,
					0.12f, // VolumeMultiplier
					1.0f  // PitchMultiplier
				);
			}
		}
	}
}
