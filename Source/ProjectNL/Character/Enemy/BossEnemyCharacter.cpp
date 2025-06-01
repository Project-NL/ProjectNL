#include "BossEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "ProjectNL/UI/Widget/Enemy/EnemyStatus.h"

ABossEnemyCharacter::ABossEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABossEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 0번 플레이어 폰 가져오기
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn || !EnemyStatusWidgetClass)
	{
		return;
	}

	const float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	if (Distance <= VisibilityDistance)
	{
		// 아직 위젯이 없으면 생성
		if (!EnemyStatusWidget)
		{
			if (APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController()))
			{
				EnemyStatusWidget = CreateWidget<UEnemyStatus>(PC, EnemyStatusWidgetClass);
				if (EnemyStatusWidget)
				{
					EnemyStatusWidget->AddToViewport();
					EnemyStatusWidget->SetBaseCharacter(this);
				}
			}
		}
	}
	else
	{
		// 범위 벗어나면 제거
		if (EnemyStatusWidget)
		{
			EnemyStatusWidget->RemoveFromParent();
			EnemyStatusWidget = nullptr;
		}
	}
}
