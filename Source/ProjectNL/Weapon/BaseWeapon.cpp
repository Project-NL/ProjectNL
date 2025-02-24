#include "BaseWeapon.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/InventoryComponent/EquipInventoryComponent.h"
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

void ABaseWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OtherActor && OtherActor != this)
	{
		// 예시로 APlayerCharacter를 플레이어 캐릭터 클래스로 가정
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
		UEquipInventoryComponent* EquipInventoryComponent=PlayerCharacter->GetEquipInventoryComponent();
		if (EquipInventoryComponent)
		{
			// 인벤토리에 아이템 추가 (인벤토리 컴포넌트의 AddItem 함수 구현에 따라 반환값 체크)
			int8 bAdded = EquipInventoryComponent->AddItemMeta(ItemMetaInfo);
			if (bAdded>0)
			{
				// 아이템 추가에 성공하면, 아이템 액터를 파괴합니다.
				Destroy();
			}
		}
	}
	
}

// TODO: 현재는 Weapon 객체에 저장되어 있으나, Manager로 옮기는 것도 고려해보면 좋을 것 같음.
void ABaseWeapon::EquipCharacterWeapon(ACharacter* Character, const bool IsMain)
{
	const FString AttachSocket = "weapon";
	const FString Position = IsMain ? "_r" : "_l";

	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
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
