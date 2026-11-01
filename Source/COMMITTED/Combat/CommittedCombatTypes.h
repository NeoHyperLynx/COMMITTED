#pragma once

#include "CoreMinimal.h"
#include "CommittedCombatTypes.generated.h"

UENUM(BlueprintType)
enum class ECommittedCombatState : uint8
{
    Ready,
    Startup,
    Active,
    Recovery,
    Deflect,
    Staggered,
    PostureBroken,
    Grabbed,
    PerformingTakedown,
    Defeated
};

UENUM(BlueprintType)
enum class ECommittedLimb : uint8
{
    None, LeftArm, RightArm, LeftLeg, RightLeg
};

UENUM(BlueprintType)
enum class EEnvironmentalSurfaceType : uint8
{
    None,
    Wall,
    Pillar,
    Ledge
};

UENUM(BlueprintType)
enum class EGrabTargetPart : uint8
{
    Head,
    Body
};

UENUM(BlueprintType)
enum class EFighterArchetype : uint8
{
    Striker,
    Grappler,
    Weapon,
    Hybrid
};

UENUM(BlueprintType)
enum class EFighterExchangeIdentity : uint8
{
    BareHandStriker,
    Grappler,
    StickWeapon,
    KickingFlow
};

USTRUCT(BlueprintType)
struct COMMITTED_API FCommittedAttack
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    FName Name = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.01"))
    float StartupSeconds = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.01"))
    float ActiveSeconds = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.01"))
    float RecoverySeconds = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.0"))
    float Range = 175.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.0", ClampMax="180.0"))
    float HalfAngleDegrees = 45.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.0"))
    float Damage = 20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack", meta=(ClampMin="0.0"))
    float PostureDamage = 18.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    ECommittedLimb TargetLimb = ECommittedLimb::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    bool bLethal = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    bool bUnblockable = false;
};
