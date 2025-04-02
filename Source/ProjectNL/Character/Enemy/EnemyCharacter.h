// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNL/Character/BaseCharacter.h"
#include "ProjectNL/UI/Widget/Enemy/EnemyStatus.h"

#include "EnemyCharacter.generated.h"

class UBaseAttributeSet;
class	UWidgetComponent;
/**
 * 
 */
UCLASS()
class PROJECTNL_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemyCharacter();
	UPROPERTY()
	TObjectPtr<UBaseAttributeSet> EnemyAttributeSet;

	// 아이템 위에 표시할 3D Widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* WidgetComponent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void Die() ;

	void WidgetComponentLookAtPlayer();
	UFUNCTION(NetMulticast,Reliable)
	void MulticastDestroy();
	UFUNCTION(Server, Reliable,WithValidation)
	void ServerDestroy();

private:
	UFUNCTION(NetMulticast, Reliable)
	void OnDamaged(const FDamagedResponse& DamagedResponse);

	UPROPERTY(EditAnywhere)
	TSubclassOf<UEnemyStatus> EnemyStatusHUDClass;

	UPROPERTY()
	UEnemyStatus* EnemyStatus;


};
