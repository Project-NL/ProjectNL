// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectNL/AnimNotify/AnimNotify_RemoveGameplayTag.h"

#include "ProjectNL/Character/BaseCharacter.h"

void UAnimNotify_RemoveGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	// 캐릭터(또는 액터)가 IAbilitySystemInterface를 구현하고 있다고 가정
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(Owner);
	if (!BaseCharacter)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = BaseCharacter->GetAbilitySystemComponent();
	if (!AbilitySystemComponent)
	{
		return;
	}

	
	// 현재 액터가 지정한 태그를 가지고 있는지 확인 후 제거
	if (AbilitySystemComponent->HasMatchingGameplayTag(TagToRemove))
	{
		// Loose Gameplay Tag(임시 태그) 제거
		AbilitySystemComponent->RemoveLooseGameplayTag(TagToRemove);

		// 또는 게임플레이 효과를 통해 태그가 부여된 경우라면
		// AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(...)
		// 와 같은 방식을 쓸 수도 있습니다.
	}
}

