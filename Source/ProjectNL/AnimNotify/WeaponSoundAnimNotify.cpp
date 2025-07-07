#include "ProjectNL/AnimNotify/WeaponSoundAnimNotify.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/Component/EquipComponent/EquipComponent.h"
#include "ProjectNL/Weapon/BaseWeapon.h"
#include "Sound/SoundAttenuation.h"  // USoundAttenuation 포함

void UWeaponSoundAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner()))
	{
		ABaseWeapon* Weapon = Owner->GetEquipComponent()->GetMainWeapon();
		if (Weapon)
		{
			USoundBase* SoundToPlay = Weapon->GetSwingSound();
			if (SoundToPlay)
			{
				FVector Location = Weapon->GetActorLocation();

				// SpawnSoundAtLocation의 마지막 인자로 AttenuationSettings를 넘겨준다.
				UGameplayStatics::SpawnSoundAtLocation(
					Weapon,                  // WorldContextObject (무기 액터나 애니메이션을 실행 중인 오브젝트)
					SoundToPlay,             // 재생할 사운드
					Location,                // 월드상의 위치
					FRotator::ZeroRotator,   // 회전값 (필요 없으면 ZeroRotator)
					0.12f,                   // VolumeMultiplier
					1.0f,                    // PitchMultiplier
					0.0f,                    // StartTime (기본 0)
					AttenuationSettings      // 이곳에 3D 감쇠 설정을 넣는다!
				);
			}
		}
	}
}
