#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CommittedCombatTypes.h"
#include "CommittedFighterDefinition.generated.h"

class UAnimMontage;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct COMMITTED_API FCommittedFighterDefinition : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Identity")
    FName RowID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Identity")
    FText FighterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Identity")
    FText TraditionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Identity")
    FText OriginRegion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Identity")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Classification")
    EFighterArchetype Archetype = EFighterArchetype::Striker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Classification")
    EFighterExchangeIdentity ExchangeIdentity = EFighterExchangeIdentity::BareHandStriker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Tags")
    FGameplayTagContainer StyleTags;

    // Soft references for animations & input contexts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Animation")
    TSoftObjectPtr<UAnimMontage> SignatureMoveMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Animation")
    TSoftObjectPtr<UAnimMontage> TakedownSetMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Input")
    TSoftObjectPtr<UInputMappingContext> InputMappingContext;

    // Posture Profile
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Posture", meta=(ClampMin="50.0"))
    float PostureMax = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Posture", meta=(ClampMin="1.0"))
    float PostureRegenRate = 18.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Posture")
    float DeflectWindowModifier = 0.f;

    // Flow Profile
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Flow", meta=(ClampMin="1.0"))
    float FlowGainRate = 12.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Flow", meta=(ClampMin="10.0"))
    float FlowMax = 100.f;

    // Moveset definitions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Moveset")
    FCommittedAttack LightAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Moveset")
    FCommittedAttack HeavyAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fighter|Moveset")
    FCommittedAttack SignatureAttack;
};
