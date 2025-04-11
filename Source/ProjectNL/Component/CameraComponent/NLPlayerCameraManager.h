// #pragma once
//
// #include "CoreMinimal.h"
// #include "Camera/PlayerCameraManager.h"
// #include "NLPlayerCameraManager.generated.h"
//
// /**
//  * APlayerCameraManager를 상속한 풀-기능 예시
//  *  - 카메라 충돌 체크
//  *  - Lock-On 보정
//  *  - 부드러운 추적(Camera Lag)
//  *  - FOV 변경
//  *  - Pitch 제한
//  */
// UCLASS()
// class YOURPROJECT_API ANLPlayerCameraManager : public APlayerCameraManager
// {
//     GENERATED_BODY()
//
// public:
//     ANLPlayerCameraManager();
//
//     /** 매 프레임 카메라 갱신 */
//     virtual void UpdateCamera(float DeltaTime) override;
//
// protected:
//     /* ------------------------------------------------
//      * 1) 기본 카메라 세팅
//      * ------------------------------------------------*/
//     /** 캐릭터(또는 Pawn)로부터 떨어진 카메라 거리 (SpringArm의 TargetArmLength 유사) */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Basic")
//     float TargetArmLength;
//
//     /** 카메라 상하(Pitch) 제한 각도 */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Basic")
//     float MinPitch;
//
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Basic")
//     float MaxPitch;
//
//     /* ------------------------------------------------
//      * 2) 카메라 충돌 체크
//      * ------------------------------------------------*/
//     /** 카메라가 벽 등에 막히면 당겨오기 위한 충돌 체크 사용 여부 */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Collision")
//     bool bDoCollisionTest;
//
//     /** 카메라 충돌을 위한 트레이스 채널 (ECC_Visibility, ECC_Camera 등) */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Collision")
//     TEnumAsByte<ECollisionChannel> CollisionChannel;
//
//     /* ------------------------------------------------
//      * 3) 부드러운 추적(Camera Lag)
//      * ------------------------------------------------*/
//     /** 카메라가 위치를 보간해서 따라올 때 사용하는 속도 */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Lag")
//     float CameraLagSpeed;
//
//     /* ------------------------------------------------
//      * 4) Lock-On(타겟 고정) 보정
//      * ------------------------------------------------*/
// public:
//     /** 현재 Lock-On 중인 대상(없으면 nullptr) */
//     UPROPERTY(BlueprintReadWrite, Category="LockOn")
//     AActor* LockOnTarget;
//
// protected:
//     /** Lock-On 상태일 때 카메라가 대상 방향으로 회전 보정할 때의 보간 속도 */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LockOn")
//     float LockOnInterpSpeed;
//
//     /* ------------------------------------------------
//      * 5) 시야각(FOV) 동적 조정
//      * ------------------------------------------------*/
// public:
//     /** 목표 FOV (보간 대상) */
//     UPROPERTY(BlueprintReadWrite, Category="Camera|FOV")
//     float TargetFOV;
//
//     /** FOV 변경 시 보간 속도 */
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|FOV")
//     float FOVInterpSpeed;
//
// protected:
//     /** 매 프레임 FOV를 보간하기 위한 현재 FOV */
//     float CurrentFOV;
//
// public:
//     /* ------------------------------------------------
//      * Lock-On 제어 함수
//      * ------------------------------------------------*/
//     UFUNCTION(BlueprintCallable, Category="LockOn")
//     void SetLockOnTarget(AActor* NewTarget);
//
//     UFUNCTION(BlueprintCallable, Category="LockOn")
//     void ClearLockOnTarget();
//
//     /* ------------------------------------------------
//      * FOV 제어 함수
//      * ------------------------------------------------*/
//     UFUNCTION(BlueprintCallable, Category="Camera|FOV")
//     void SetCameraFOV(float NewFOV);
//
//     /* ------------------------------------------------
//      * 디버그 표시
//      * ------------------------------------------------*/
//     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
//     bool bDrawDebug;
// };
