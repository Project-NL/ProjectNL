#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectNL/Helper/UtilHelper.h"
#include "ProjectNL/Item/SpawnableItem.h"
#include "BaseWeapon.generated.h"

class UGameplayEffect;
enum class EUWeaponType : uint8;
enum class EUEquippedHandType : uint8;
enum class EWeaponAttachPosition : uint8;

UCLASS()
class PROJECTNL_API ABaseWeapon : public ASpawnableItem
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	void EquipCharacterWeapon(ACharacter* Character, const bool IsMain);
	void UnEquipCharacterWeapon(const bool IsMain);
	void UnEquipCharacterWeapon(ACharacter* Character, const bool IsMain);
	void SwapTwoHandWeapon();

	TSet<AActor*>& GetHitActorsReference();
	GETTER(EUEquippedHandType, EquippedHandType)
	GETTER(EWeaponAttachPosition, AttachPosition)
	GETTER(EUWeaponType, WeaponType)
	GETTER(USkeletalMeshComponent*, WeaponSkeleton)
	GETTER(TSubclassOf<UGameplayEffect>, AttackEffect)

	GETTER_SETTER(FVector, PrevStartLocation)
	GETTER_SETTER(FVector, PrevEndLocation)

	
	
protected:
	virtual void BeginPlay() override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void Interact(AActor* Actor) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetPhysicsAndCollision();

	// 서버에서만 실행되는 Interact 메서드
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerWeaponInteract(AActor* InteractingActor);
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Assets
	, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Assets
		, meta = (AllowPrivateAccess = "true"))
	EUEquippedHandType EquippedHandType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Assets
		, meta = (AllowPrivateAccess = "true"))
	EUWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Assets
		, meta = (AllowPrivateAccess = "true"))
	EWeaponAttachPosition AttachPosition;

	UPROPERTY(EditDefaultsOnly, Category = Ability, meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameplayEffect> AttackEffect;
	
	UPROPERTY()
	TSet<AActor*> HitActors;

	// 캐릭터별로 관리할 변수 선언
	FVector PrevStartLocation;
	FVector PrevEndLocation;

	bool bAddInventory;
};
