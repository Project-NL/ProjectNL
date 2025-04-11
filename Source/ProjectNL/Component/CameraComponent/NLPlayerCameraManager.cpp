// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "ProjectNL/Component/CameraComponent/NLPlayerCameraManager.h"
// #include "GameFramework/Pawn.h"
// #include "GameFramework/Controller.h"
// #include "Engine/World.h"
// #include "DrawDebugHelpers.h"
//
// ANLPlayerCameraManager::ANLPlayerCameraManager()
// {
//     // 1) 기본 카메라 세팅
//     TargetArmLength = 300.0f;   // 캐릭터 뒤로 300 떨어진 지점
//     MinPitch = -80.0f;
//     MaxPitch =  10.0f;
//
//     // 2) 카메라 충돌 체크 기본값
//     bDoCollisionTest = true;
//     CollisionChannel = ECC_Visibility;  // 필요 시 ECC_Camera 등으로 변경
//
//     // 3) 부드러운 추적(Camera Lag)
//     CameraLagSpeed = 10.0f;
//
//     // 4) Lock-On
//     LockOnTarget = nullptr;
//     LockOnInterpSpeed = 5.0f;
//
//     // 5) 시야각(FOV)
//     TargetFOV = 90.0f;
//     CurrentFOV = TargetFOV;
//     FOVInterpSpeed = 5.0f;
//
//     // 디버그 표시
//     bDrawDebug = false;
//
//     // APlayerCameraManager가 기본적으로 사용하는 FOV
//     DefaultFOV = CurrentFOV;  
// }
//
// void ANLPlayerCameraManager::UpdateCamera(float DeltaTime)
// {
//     Super::UpdateCamera(DeltaTime);
//
//     // 1) Pawn(플레이어 캐릭터) 확인
//     APawn* MyPawn = PCOwner ? PCOwner->GetPawn() : nullptr;
//     if (!MyPawn) return;
//
//     // 2) 플레이어 컨트롤러의 회전값을 가져온다 (Yaw/Pitch)
//     FRotator ControllerRot = PCOwner->GetControlRotation();
//     // Pitch 제한
//     ControllerRot.Pitch = FMath::Clamp(ControllerRot.Pitch, MinPitch, MaxPitch);
//
//     // 3) Lock-On 상태라면, 카메라가 대상 방향으로 서서히 회전하도록 처리
//     if (LockOnTarget)
//     {
//         FVector PawnLoc = MyPawn->GetActorLocation();
//         FVector TargetLoc = LockOnTarget->GetActorLocation();
//
//         FVector ToTarget = TargetLoc - PawnLoc;
//         // 필요 시, Z도 그대로 두지 않고 Pitch까지 맞추거나, 상대 높이를 조절할 수 있음
//         FRotator TargetRot = ToTarget.Rotation();
//
//         // Yaw 보간
//         float DeltaYaw = FMath::FindDeltaAngleDegrees(ControllerRot.Yaw, TargetRot.Yaw);
//         float NewYaw = ControllerRot.Yaw + DeltaYaw * FMath::Clamp(DeltaTime * LockOnInterpSpeed, 0.f, 1.f);
//
//         // Pitch 보간 (원한다면 Lock-On 시에도 상하 각도 맞춤)
//         float DeltaPitch = FMath::FindDeltaAngleDegrees(ControllerRot.Pitch, TargetRot.Pitch);
//         float NewPitch = ControllerRot.Pitch + DeltaPitch * FMath::Clamp(DeltaTime * LockOnInterpSpeed, 0.f, 1.f);
//
//         // 다시 Pitch 제한 적용
//         NewPitch = FMath::Clamp(NewPitch, MinPitch, MaxPitch);
//
//         ControllerRot.Yaw   = NewYaw;
//         ControllerRot.Pitch = NewPitch;
//     }
//
//     // 4) 카메라(Desired) 위치 계산
//     //    PawnLocation에서 뒤쪽(TargetArmLength)으로 이동
//     FVector PawnLocation = MyPawn->GetActorLocation();
//     FRotator DesiredRot   = ControllerRot;
//     FVector DesiredLoc    = PawnLocation - DesiredRot.Vector() * TargetArmLength;
//
//     // (원한다면 캐릭터 머리 높이만큼 올려줄 수도 있음)
//     // DesiredLoc.Z += 50.0f; // 예시
//
//     // 5) 충돌 체크
//     if (bDoCollisionTest)
//     {
//         FHitResult Hit;
//         FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(ANLPlayerCameraManager), false, MyPawn);
//
//         bool bHit = GetWorld()->LineTraceSingleByChannel(
//             Hit,
//             PawnLocation,  
//             DesiredLoc,
//             CollisionChannel,
//             TraceParams
//         );
//
//         if (bHit)
//         {
//             // 충돌 위치까지 카메라를 당겨옴
//             DesiredLoc = Hit.Location;
//         }
//
//         // 디버그 표시
//         if (bDrawDebug)
//         {
//             FColor LineColor = bHit ? FColor::Red : FColor::Green;
//             DrawDebugLine(GetWorld(), PawnLocation, DesiredLoc, LineColor, false, 0.f, 0, 2.f);
//         }
//     }
//
//     // 6) 부드러운 추적(Camera Lag)
//     //    현재 카메라 위치 -> DesiredLoc 로 보간
//     FVector CurrentLoc = GetCameraLocation();
//     FVector NewLoc = FMath::VInterpTo(CurrentLoc, DesiredLoc, DeltaTime, CameraLagSpeed);
//
//     // 회전 보간 (RInterpTo) - 필요 없으면 그냥 DesiredRot를 써도 됨
//     FRotator CurrentRot = GetCameraRotation();
//     FRotator NewRot = FMath::RInterpTo(CurrentRot, DesiredRot, DeltaTime, CameraLagSpeed);
//
//     
//     // 최종 카메라 위치/회전 반영
//     SetCameraLocation(NewLoc);
//     SetCameraRotation(NewRot);
//
//     // 7) FOV 보간
//     if (!FMath::IsNearlyEqual(CurrentFOV, TargetFOV, 0.01f))
//     {
//         CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
//         SetFOV(CurrentFOV);
//     }
// }
//
// /* ------------------------------------------------
//  * Lock-On 세팅
//  * ------------------------------------------------*/
// void ANLPlayerCameraManager::SetLockOnTarget(AActor* NewTarget)
// {
//     LockOnTarget = NewTarget;
// }
//
// void ANLPlayerCameraManager::ClearLockOnTarget()
// {
//     LockOnTarget = nullptr;
// }
//
// /* ------------------------------------------------
//  * FOV 세팅
//  * ------------------------------------------------*/
// void ANLPlayerCameraManager::SetCameraFOV(float NewFOV)
// {
//     TargetFOV = NewFOV;
// }
