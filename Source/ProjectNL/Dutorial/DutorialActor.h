#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DutorialActor.generated.h"

class UBoxComponent;
class UUserWidget;

UCLASS()
class PROJECTNL_API ADutorialActor : public AActor
{
	GENERATED_BODY()
	
public:
	ADutorialActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "Tutorial")
	TArray<TSubclassOf<UUserWidget>> TutorialWidgetClasses;

	UPROPERTY()
	UUserWidget* ActiveWidget;

	UPROPERTY()
	bool bHasBeenShown = false;

	int32 CurrentTutorialIndex = 0;

	FTimerHandle TutorialTimerHandle;

	// 튜토리얼 교체 함수
	void ShowNextTutorial();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
