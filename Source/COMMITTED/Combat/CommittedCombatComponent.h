#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommittedCombatComponent.generated.h"

UENUM(BlueprintType)
enum class ECommittedCombatState : uint8
{
    Ready, Startup, Active, Recovery, Reversal, Staggered, Defeated
};

UENUM(BlueprintType)
enum class ECommittedLimb : uint8
{
    None, LeftArm, RightArm, LeftLeg, RightLeg
};

USTRUCT(BlueprintType)
struct FCommittedAttack
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
    ECommittedLimb TargetLimb = ECommittedLimb::None;

    // A clean, unreversed hit defeats the opponent. Use this sparingly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
    bool bLethal = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommittedCombatEvent, ECommittedCombatState, NewState, FName, MoveName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommittedHitEvent, ECommittedLimb, Limb, bool, bLethal);

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class COMMITTED_API UCommittedCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCommittedCombatComponent();

    UPROPERTY(BlueprintAssignable, Category="COMMITTED|Combat")
    FCommittedCombatEvent OnCombatStateChanged;

    UPROPERTY(BlueprintAssignable, Category="COMMITTED|Combat")
    FCommittedHitEvent OnHitReceived;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat", meta=(ClampMin="1.0"))
    float MaxHealth = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat", meta=(ClampMin="0.01"))
    float ReversalWindowSeconds = 0.22f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat", meta=(ClampMin="0.01"))
    float ReversalRecoverySeconds = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat", meta=(ClampMin="0.01"))
    float ReversedAttackerStaggerSeconds = 0.8f;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    float Health = 100.f;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    ECommittedCombatState State = ECommittedCombatState::Ready;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    FCommittedAttack CurrentAttack;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    TArray<ECommittedLimb> ImpairedLimbs;

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool StartAttack(const FCommittedAttack& Attack);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool StartReversal();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    void ResetCombat();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle PhaseTimer;
    TSet<UCommittedCombatComponent*> HitThisAttack;

    void SetState(ECommittedCombatState NewState);
    void EnterActive();
    void EnterRecovery();
    void EnterReady();
    void EnterReversalRecovery();
    void ScanForTargets();
    void ReceiveAttack(UCommittedCombatComponent* Attacker, const FCommittedAttack& Attack);
    void Stagger(float Duration);
};
