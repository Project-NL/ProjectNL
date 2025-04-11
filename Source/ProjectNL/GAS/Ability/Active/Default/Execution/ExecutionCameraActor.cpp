// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/GAS/Ability/Active/Default/Execution/ExecutionCameraActor.h"

#include "Camera/CameraComponent.h"

// Sets default values
AExecutionCameraActor::AExecutionCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick =	false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
}

// Called when the game starts or when spawned
void AExecutionCameraActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExecutionCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExecutionCameraActor::SetExecutionCameraTransform(const FVector& NewLocation, const FRotator& NewRotation)
{
	SetActorLocationAndRotation(NewLocation, NewRotation);
}

