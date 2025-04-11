// #include "AiGameplaycueAnimNotify.h"
// #include "AbilitySystemBlueprintLibrary.h"
// #include "GameFramework/Actor.h"
// #include "Kismet/KismetSystemLibrary.h"
// #include "ProjectNL/Character/BaseCharacter.h"
//
// void UAiGameplaycueAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
// {
// 	if (!MeshComp)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UAiGameplaycueAnimNotify: MeshComp is null."));
// 		return;
// 	}
//
// 	AActor* Owner = MeshComp->GetOwner();
// 	if (!Owner)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UAiGameplaycueAnimNotify: MeshComp has no valid owner."));
// 		return;
// 	}
// 	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(Owner);
// 	if (!BaseCharacter)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UAiGameplaycueAnimNotify: BaseCharacter has no valid owner."));
// 		return;
// 	}
// 	if (!GameplayCueTag.IsValid())
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UAiGameplaycueAnimNotify: GameplayCueTag is not valid."));
// 		return;
// 	}
//
// 	// GameplayCue 실행
// 	FGameplayEffectContextHandle EffectContext;
// 	FVector CueLocation = MeshComp->GetComponentLocation() + CueLocationOffset;
// 	//UAbilitySystemBlueprintLibrary::ExecuteGameplayCue(Owner, GameplayCueTag, EffectContext);
//
// 	UAbilitySystemComponent* ASC = BaseCharacter->GetAbilitySystemComponent();
// 	if (!ASC)
// 	{
// 		return;
// 	}
// 	
// 	
// 	FGameplayCueParameters Param;
// 	Param.SourceObject = this;
// 	Param.Instigator = Owner;
// 	Param.Location = ImpactPoint;
// 	ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Test")), Param);
// 	//ASC->PlayMontageSimulated(HitAnimMontage, 1.0f);
// 	
// 	// Debug 출력
// 	if (bDebug)
// 	{
// 		UKismetSystemLibrary::PrintString(MeshComp->GetWorld(), FString::Printf(TEXT("Triggered GameplayCue: %s at Location: %s"),
// 			*GameplayCueTag.ToString(), *CueLocation.ToString()));
// 		UE_LOG(LogTemp, Log, TEXT("UAiGameplaycueAnimNotify: Triggered GameplayCue: %s at Location: %s"),
// 			*GameplayCueTag.ToString(), *CueLocation.ToString());
// 	}
// }
