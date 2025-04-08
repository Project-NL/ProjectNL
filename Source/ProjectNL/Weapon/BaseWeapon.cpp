#include "BaseWeapon.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "ProjectNL/Character/Player/PlayerCharacter.h"
#include "ProjectNL/Component/InventoryComponent/EquipInventoryComponent.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "ProjectNL/Player/BasePlayerController.h"


ABaseWeapon::ABaseWeapon()
{
	SetReplicates(true);
	WeaponSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("Weapon Bone"));
	WeaponSkeleton->SetupAttachment(RootComponent);
	//
	// WeaponSkeleton->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// WeaponSkeleton->SetSimulatePhysics(true);
	// WeaponSkeleton->SetCollisionObjectType(ECC_PhysicsBody);
	// WeaponSkeleton->SetNotifyRigidBodyCollision(true); // 충돌 이벤트 받기
	//
	EquippedHandType = EUEquippedHandType::Empty;
	AttachPosition = EWeaponAttachPosition::Back;

	bcheckitem=false;
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void ABaseWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);

	
}

void ABaseWeapon::Interact(AActor* Actor)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Actor);
	UEquipInventoryComponent* EquipInventoryComponent=PlayerCharacter->GetEquipInventoryComponent();
	int8 nAdded = EquipInventoryComponent->AddItemMeta(ItemMetaInfo);
	ServerInteract(Actor);
}

void ABaseWeapon::Multicast_SetPhysicsAndCollision_Implementation()
{
	WeaponSkeleton->SetSimulatePhysics(false); // 물리 시뮬레이션 끔
	WeaponSkeleton->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 충돌은 오버랩만 가능
	WeaponSkeleton->SetCollisionObjectType(ECC_WorldDynamic); // 기본 오브젝트 타입
	WeaponSkeleton->SetNotifyRigidBodyCollision(false); // 충돌 이벤트 안 받음

}


void ABaseWeapon::ServerWeaponInteract_Implementation(AActor* InteractingActor)
{

	MulticastDestroyItem();
}

bool ABaseWeapon::ServerWeaponInteract_Validate(AActor* InteractingActor)
{
	return true;
}

// TODO: 현재는 Weapon 객체에 저장되어 있으나, Manager로 옮기는 것도 고려해보면 좋을 것 같음.
void ABaseWeapon::EquipCharacterWeapon(ACharacter* Character, const bool IsMain)
{
	
	
	const FString AttachSocket = "weapon";
	const FString Position = IsMain ? "_r" : "_l";
	AttachToComponent(Character->GetMesh()
								, FAttachmentTransformRules::SnapToTargetIncludingScale
								, *(AttachSocket + Position));
	bcheckitem=true;
//	Multicast_SetPhysicsAndCollision();
	Multicast_SetCollision();
	if (HasAuthority())
	{
		// 물리 꺼주기
		
		//WeaponSkeleton->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Error, TEXT("EquipCharacterWeapon()이 서버에서 실행되었습니다! 문제 발생 가능"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquipCharacterWeapon()이 클라이언트에서 실행되었습니다! 문제 발생 가능"));
	}
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
