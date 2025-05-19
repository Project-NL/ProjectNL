// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNL_API APlayGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	
	virtual void PostLogin(APlayerController* NewPlayer)override;
};
