#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DevotedCombatComponent.generated.h"

UENUM(BlueprintType)
enum class EDevotedCombatState : uint8
{
    Ready, Startup, Active, Recovery, Reversal, Staggered, Defeated
};

UENUM(BlueprintType)
enum class EDevotedLimb : uint8
{
    None, LeftArm, RightArm, LeftLeg, RightLeg
};

USTRUCT(BlueprintType)
struct FDevotedAttack
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    EDevotedLimb TargetLimb = EDevotedLimb::None;

    // A clean, unreversed hit defeats the opponent. Use this sparingly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    bool bLethal = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDevotedCombatEvent, EDevotedCombatState, NewState, FName, MoveName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDevotedHitEvent, EDevotedLimb, Limb, bool, bLethal);

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class DEVOTED_API UDevotedCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDevotedCombatComponent();

    UPROPERTY(BlueprintAssignable, Category="DEVOTED|Combat")
    FDevotedCombatEvent OnCombatStateChanged;

    UPROPERTY(BlueprintAssignable, Category="DEVOTED|Combat")
    FDevotedHitEvent OnHitReceived;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Combat", meta=(ClampMin="1.0"))
    float MaxHealth = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Combat", meta=(ClampMin="0.01"))
    float ReversalWindowSeconds = 0.22f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Combat", meta=(ClampMin="0.01"))
    float ReversalRecoverySeconds = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Combat", meta=(ClampMin="0.01"))
    float ReversedAttackerStaggerSeconds = 0.8f;

    UPROPERTY(BlueprintReadOnly, Category="DEVOTED|Combat")
    float Health = 100.f;

    UPROPERTY(BlueprintReadOnly, Category="DEVOTED|Combat")
    EDevotedCombatState State = EDevotedCombatState::Ready;

    UPROPERTY(BlueprintReadOnly, Category="DEVOTED|Combat")
    FDevotedAttack CurrentAttack;

    UPROPERTY(BlueprintReadOnly, Category="DEVOTED|Combat")
    TArray<EDevotedLimb> ImpairedLimbs;

    UFUNCTION(BlueprintCallable, Category="DEVOTED|Combat")
    bool StartAttack(const FDevotedAttack& Attack);

    UFUNCTION(BlueprintCallable, Category="DEVOTED|Combat")
    bool StartReversal();

    UFUNCTION(BlueprintCallable, Category="DEVOTED|Combat")
    void ResetCombat();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle PhaseTimer;
    TSet<UDevotedCombatComponent*> HitThisAttack;

    void SetState(EDevotedCombatState NewState);
    void EnterActive();
    void EnterRecovery();
    void EnterReady();
    void EnterReversalRecovery();
    void ScanForTargets();
    void ReceiveAttack(UDevotedCombatComponent* Attacker, const FDevotedAttack& Attack);
    void Stagger(float Duration);
};
