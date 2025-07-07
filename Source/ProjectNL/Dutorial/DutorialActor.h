#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DutorialActor.generated.h"

class UBoxComponent;
class UUserWidget;
class USoundBase;

UCLASS()
class PROJECTNL_API ADutorialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADutorialActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UUserWidget>> TutorialWidgetClasses;

	UPROPERTY()
	UUserWidget* ActiveWidget;

	UPROPERTY()
	int32 CurrentTutorialIndex = 0;

	void NextTutorialStep();

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ShowNextTutorial();

public:
	// …
	/** 튜토리얼 넘어갈 때 재생할 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial|Sound")
	USoundBase* NextTutorialSound;

	/** 플레이어가 Overlap 시 적용할 Gameplay Effect 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TSubclassOf<class UGameplayEffect> OverlapGameplayEffect;
};
