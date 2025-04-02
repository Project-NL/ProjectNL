// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExecutionCameraActor.generated.h"

class UCameraComponent;

UCLASS()
class PROJECTNL_API AExecutionCameraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExecutionCameraActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	/** 카메라의 위치와 회전을 설정 */
	void SetExecutionCameraTransform(const FVector& NewLocation, const FRotator& NewRotation);
};
