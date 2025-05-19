// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGameModeBase.h"

#include "Kismet/GameplayStatics.h"

void APlayGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
    
	// 로그인 처리 후 엔진이 DefaultPawn을 Spawn&Possess 했으므로,
	// NewPlayer->GetPawn()으로 가져와서 위치만 옮깁니다.
	APawn* PlayerPawn = NewPlayer->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}

	// 호스트 Pawn 위치 & 회전
	APlayerController* HostPC = UGameplayStatics::GetPlayerController(this, 0);
	if (!HostPC || !HostPC->GetPawn())
	{
		return;
	}
	FVector HostLoc  = HostPC->GetPawn()->GetActorLocation();
	FRotator HostRot = HostPC->GetPawn()->GetActorRotation();

	// 호스트의 오른쪽(+) X 방향으로 200cm 떨어진 위치
	FVector SpawnOffset = HostRot.RotateVector(FVector(200.f, 0.f, 0.f));
	FVector TargetLoc   = HostLoc + SpawnOffset;

	// 1) 간단하게 Transform 설정
	PlayerPawn->SetActorLocationAndRotation(TargetLoc, HostRot);

	// **또는** 물리와 충돌을 무시하고 순간이동하려면 TeleportTo 사용
	// PlayerPawn->TeleportTo(TargetLoc, HostRot);
}
