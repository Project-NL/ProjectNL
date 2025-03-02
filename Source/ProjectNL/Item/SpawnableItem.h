#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectNL/Interface/InteractableInterface.h"
#include "ProjectNL/Interface/ItemInterface.h"
#include "ProjectNL/DataTable/ItemInfoData.h"
#include "SpawnableItem.generated.h"

class ABasePlayerController;
class UBoxComponent;
class UWidgetComponent;
class APlayerCharacter;
UCLASS()
class PROJECTNL_API ASpawnableItem : public AActor, public IItemInterface, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 충돌 발생 시 호출될 함수
	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult) ;

	// EndOverlap 이벤트
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void Tick(float DeltaSeconds) override;

	void AcquireWidgetComponentLookAtPlayer();
public:
	virtual void Interact(AActor* Actor) override;
	virtual void UseItem() override;

	FItemMetaInfo* GetItemMetainfo();
protected:
	// 아이템 데이터
	UPROPERTY(EditAnywhere)
	FItemMetaInfo ItemMetaInfo;
	
	// 충돌 처리를 위한 박스 컴포넌트
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	// 아이템 위에 표시할 3D Widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* AcquireWidgetComponent;

	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComp;
	// 현재 아이템과 겹쳐있는 플레이어 (상호작용을 위해)
	UPROPERTY()
	APlayerCharacter* OverlappingPlayer;


	UPROPERTY()
	ABasePlayerController *OverlappingPlayerController;

};
