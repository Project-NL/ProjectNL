// Fill out your copyright notice in the Description page of Project Settings.


#include "PotionItem.h"

#include "ProjectNL/Character/Player/PlayerCharacter.h"

APotionItem::APotionItem()
{
	PotionSkeleton = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("Potion Bone"));
	PotionSkeleton->SetupAttachment(RootComponent);
}

void APotionItem::UseItem(APlayerCharacter* playerCharacter)//포션을 먹어 채력을 사용합니다
{
	if (HasAuthority())
	ServerUseItem_Implementation(playerCharacter);
	
	Super::UseItem(playerCharacter);
}

void APotionItem::ServerUseItem_Implementation(APlayerCharacter* playerCharacter)
{
	if (!DrinkPotionAbility)
	{
		return;
	}
	
	UAbilitySystemComponent* AbilitySystemComponent = playerCharacter->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return;
	}
	// 어빌리티를 실행 시도
	FGameplayAbilitySpec AbilitySpec = playerCharacter->GetAbilitySystemComponent()->BuildAbilitySpecFromClass(DrinkPotionAbility, 1, INDEX_NONE);
	if(!AbilitySpec.Ability)
	{
		return ;
	}
	
	

	//AbilitySystemComponent->GetActivatableAbilities();
	
	FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
	UGameplayAbility* ActivatedAbility = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle)->GetPrimaryInstance();
	UGA_DrinkPotion* GADrinkPotion= Cast<UGA_DrinkPotion>(ActivatedAbility);
	GADrinkPotion->SetPotionItem(this);
	bool bActivated = AbilitySystemComponent->TryActivateAbility(AbilityHandle);
	if (!bActivated)
	{
		return;
	}
	//SetActorEnableCollision(false); // 충돌 비활성화
				
	AttachToComponent(Cast<APlayerCharacter>(playerCharacter)->GetMesh(),
	FAttachmentTransformRules::SnapToTargetIncludingScale
						, FName(TEXT("LeftHandSocket")));
	
	
}
bool APotionItem::ServerUseItem_Validate(APlayerCharacter* playerCharacter)
{
	return true;
}