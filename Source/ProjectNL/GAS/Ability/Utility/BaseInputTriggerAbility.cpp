#include "BaseInputTriggerAbility.h"
#include "ProjectNL/Helper/EnumHelper.h"
#include "AbilitySystemComponent.h"

UBaseInputTriggerAbility::UBaseInputTriggerAbility(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InputID = EInputIDType::None;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBaseInputTriggerAbility::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	SetupEnhancedInputBindings(ActorInfo, Spec);
}

void UBaseInputTriggerAbility::SetupEnhancedInputBindings(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (IsValid(ActorInfo->AvatarActor.Get()))
	{
		if (const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
		{
			if (const AController* PawnController = AvatarPawn->GetController())
			{
				if (UEnhancedInputComponent* EnhancedInputComponent = Cast<
					UEnhancedInputComponent>(PawnController->InputComponent.Get()))
				{
					if (UBaseInputTriggerAbility* AbilityInstance = Cast<
						UBaseInputTriggerAbility>(Spec.Ability))
					{
						if (IsValid(AbilityInstance->ActivationInputAction))
						{
							EnhancedInputComponent->BindAction(
								ActivationInputAction, ETriggerEvent::Triggered, AbilityInstance
								, &ThisClass::OnAbilityInputPressed, ActorInfo);

							EnhancedInputComponent->BindAction(
								ActivationInputAction, ETriggerEvent::Completed, AbilityInstance
								, &ThisClass::OnAbilityInputReleased, ActorInfo);
						}
					}
				}
			}
		}
	}
}

void UBaseInputTriggerAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle
	, const FGameplayAbilityActorInfo* ActorInfo
	, const FGameplayAbilityActivationInfo ActivationInfo
	, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility
										, bWasCancelled);
	if (const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
	{
		if (const AController* PawnController = AvatarPawn->GetController())
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<
				UEnhancedInputComponent>(PawnController->InputComponent.Get()))
			{
				EnhancedInputComponent->RemoveBindingByHandle(TriggeredEventHandle);
			}
		}
		TriggeredEventHandle = -1;
	}
}

void UBaseInputTriggerAbility::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (IsValid(ActorInfo->AvatarActor.Get()))
	{
		if (const APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
		{
			if (const AController* PawnController = AvatarPawn->GetController())
			{
				if (UEnhancedInputComponent* EnhancedInputComponent = Cast<
					UEnhancedInputComponent>(PawnController->InputComponent))
				{
					EnhancedInputComponent->ClearBindingsForObject(Spec.Ability.Get());
				}
			}
		}
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UBaseInputTriggerAbility::OnAbilityInputPressed(
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (const ABaseCharacter* Owner = Cast<
		ABaseCharacter>(ActorInfo->AvatarActor))
	{
		Owner->GetAbilitySystemComponent()->AbilityLocalInputPressed(
			static_cast<uint32>(InputID));
	}
}

void UBaseInputTriggerAbility::OnAbilityInputReleased(
	const FGameplayAbilityActorInfo* ActorInfo)
{
	if (const ABaseCharacter* Owner = Cast<
		ABaseCharacter>(ActorInfo->AvatarActor))
	{
		Owner->GetAbilitySystemComponent()->AbilityLocalInputReleased(
			static_cast<uint32>(InputID));
		FGameplayAbilitySpec* Spec = Owner->GetAbilitySystemComponent()->FindAbilitySpecFromInputID(static_cast<uint8>(InputID));
		Owner->GetAbilitySystemComponent()->ServerSetInputReleased(Spec->Handle);
	}
}
