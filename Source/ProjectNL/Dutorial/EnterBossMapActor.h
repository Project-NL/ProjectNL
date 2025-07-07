// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnterBossMapActor.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTNL_API AEnterBossMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnterBossMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Box Collision 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	UBoxComponent* CollisionBox;

	/** 오버랩 시작 시 호출될 함수 */
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/** 플레이어가 Overlap 시 적용할 Gameplay Effect 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TSubclassOf<class UGameplayEffect> OverlapGameplayEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};