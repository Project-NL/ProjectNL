#include "BaseWeapon.h"

#include "GameFramework/Character.h"
#include "ProjectNL/Helper/EnumHelper.h"


ABaseWeapon::ABaseWeapon()
{
	WeaponSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("Weapon Bone"));
	WeaponSkeleton->SetupAttachment(RootComponent);
	
	EquippedHandType = EUEquippedHandType::Empty;
	AttachPosition = EWeaponAttachPosition::Back;
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// TODO: 현재는 Weapon 객체에 저장되어 있으나, Manager로 옮기는 것도 고려해보면 좋을 것 같음.
void ABaseWeapon::EquipCharacterWeapon(ACharacter* Character, const bool IsMain)
{
	const FString AttachSocket = "weapon";
	const FString Position = IsMain ? "_r" : "_l";

	AttachToComponent(Character->GetMesh()
										, FAttachmentTransformRules::SnapToTargetIncludingScale
										, *(AttachSocket + Position));
}

void ABaseWeapon::UnEquipCharacterWeapon(const bool IsMain)
{
	if (const ACharacter* Character = Cast<ACharacter>(GetAttachParentActor()))
	{
		const FString Position = IsMain ? "_r" : "_l";
		const FString AttachSocket = "weapon_" +
			FEnumHelper::GetClassEnumKeyAsString(GetAttachPosition()).ToLower();

		AttachToComponent(Character->GetMesh()
											, FAttachmentTransformRules::SnapToTargetIncludingScale
											, *(AttachSocket + Position));
	}
}

void ABaseWeapon::UnEquipCharacterWeapon(ACharacter* Character, const bool IsMain)
{
	const FString Position = IsMain ? "_r" : "_l";
	const FString AttachSocket = "weapon_" + FEnumHelper::GetClassEnumKeyAsString(
		GetAttachPosition()).ToLower();

	AttachToComponent(Character->GetMesh()
										, FAttachmentTransformRules::SnapToTargetIncludingScale
										, *(AttachSocket + Position));
}

void ABaseWeapon::SwapTwoHandWeapon()
{
	if (GetEquippedHandType() != EUEquippedHandType::TwoHand)
	{
		return;
	}
	if (const ACharacter* Character = Cast<ACharacter>(GetAttachParentActor()))
	{
		AttachToComponent(Character->GetMesh()
											, FAttachmentTransformRules::SnapToTargetIncludingScale
											, "weapon_twoHand");
	}
}

TSet<AActor*>& ABaseWeapon::GetHitActorsReference()
{
	return HitActors;
}
