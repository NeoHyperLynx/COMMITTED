#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommittedCombatTypes.h"
#include "CommittedCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommittedCombatEvent, ECommittedCombatState, NewState, FName, MoveName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommittedHitEvent, ECommittedLimb, Limb, bool, bLethal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCommittedEnvironmentalTakedownEvent, EEnvironmentalSurfaceType, Surface, AActor*, TargetActor);

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

    UPROPERTY(BlueprintAssignable, Category="COMMITTED|Combat")
    FCommittedEnvironmentalTakedownEvent OnEnvironmentalTakedown;

    // --- Health, Posture, and Flow ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat", meta=(ClampMin="1.0"))
    float MaxHealth = 100.f;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    float Health = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Posture", meta=(ClampMin="1.0"))
    float MaxPosture = 100.f;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Posture")
    float Posture = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Posture")
    float PostureRegenRate = 18.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Posture")
    float PostureRegenDelay = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Flow", meta=(ClampMin="1.0"))
    float MaxFlow = 100.f;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Flow")
    float Flow = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Flow")
    float FlowGainRate = 12.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Flow")
    float FlowDecayRate = 6.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Flow")
    float FlowDecayDelay = 3.0f;

    // --- Deflect Windows & Timers ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Deflect", meta=(ClampMin="0.01"))
    float PerfectDeflectWindowSeconds = 0.14f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Deflect", meta=(ClampMin="0.01"))
    float DeflectRecoverySeconds = 0.32f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Combat|Deflect", meta=(ClampMin="0.01"))
    float AttackerDeflectedStaggerSeconds = 0.85f;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    ECommittedCombatState State = ECommittedCombatState::Ready;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    FCommittedAttack CurrentAttack;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat")
    TArray<ECommittedLimb> ImpairedLimbs;

    // --- Environmental & Grab State ---
    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Environmental")
    EEnvironmentalSurfaceType NearbyEnvironmentalSurface = EEnvironmentalSurfaceType::None;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Environmental")
    FVector NearbySurfacePoint = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Environmental")
    FVector NearbySurfaceNormal = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Grab")
    TWeakObjectPtr<UCommittedCombatComponent> GrabbedOpponent;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Grab")
    TWeakObjectPtr<UCommittedCombatComponent> GrabbedByOpponent;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Crowd")
    bool bHasAttackToken = false;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Tradition")
    EFighterExchangeIdentity ActiveExchangeIdentity = EFighterExchangeIdentity::BareHandStriker;

    // Disarm state
    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Tradition")
    bool bIsDisarmed = false;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Combat|Tradition")
    float DisarmTimer = 0.f;

    // --- Actions ---
    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool StartAttack(const FCommittedAttack& Attack);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool StartDeflect();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool TryEnvironmentalTakedown();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool TryDedicatedGrab(EGrabTargetPart TargetPart = EGrabTargetPart::Head);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool TryGrabThrow(FVector ThrowDirection);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    void BreakGrab();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    void AddPosture(float Amount);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    void AddFlow(float Amount);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    void ResetCombat();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Combat")
    bool ScanEnvironmentalSurfaces(float MaxRadius = 200.f);

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

private:
    FTimerHandle PhaseTimer;
    TSet<UCommittedCombatComponent*> HitThisAttack;

    float PostureRegenTimer = 0.f;
    float FlowDecayTimer = 0.f;
    float DeflectActiveTime = 0.f;
    float GrabTimer = 0.f;
    int32 ConsecutiveDeflects = 0;
    float ConsecutiveDeflectResetTimer = 0.f;

    void SetState(ECommittedCombatState NewState);
    void EnterActive();
    void EnterRecovery();
    void EnterReady();
    void EnterDeflectRecovery();
    void EnterPostureBroken();
    void RecoverFromPostureBroken();
    void ScanForTargets();
    void ReceiveAttack(UCommittedCombatComponent* Attacker, const FCommittedAttack& Attack);
    void DeflectAttacker(UCommittedCombatComponent* Attacker);
    void Stagger(float Duration);
};
