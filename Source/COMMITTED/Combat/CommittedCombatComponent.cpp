#include "CommittedCombatComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "TimerManager.h"

UCommittedCombatComponent::UCommittedCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCommittedCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
}

void UCommittedCombatComponent::SetState(ECommittedCombatState NewState)
{
    State = NewState;
    OnCombatStateChanged.Broadcast(State, CurrentAttack.Name);
}

bool UCommittedCombatComponent::StartAttack(const FCommittedAttack& Attack)
{
    if (State != ECommittedCombatState::Ready || !GetWorld() ||
        Attack.StartupSeconds <= 0.f || Attack.ActiveSeconds <= 0.f ||
        Attack.RecoverySeconds <= 0.f || Attack.Range <= 0.f)
    {
        return false;
    }

    CurrentAttack = Attack;
    HitThisAttack.Reset();
    SetState(ECommittedCombatState::Startup);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UCommittedCombatComponent::EnterActive, Attack.StartupSeconds, false);
    return true;
}

void UCommittedCombatComponent::EnterActive()
{
    SetState(ECommittedCombatState::Active);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UCommittedCombatComponent::EnterRecovery, CurrentAttack.ActiveSeconds, false);
    ScanForTargets();
}

void UCommittedCombatComponent::EnterRecovery()
{
    SetState(ECommittedCombatState::Recovery);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UCommittedCombatComponent::EnterReady, CurrentAttack.RecoverySeconds, false);
}

void UCommittedCombatComponent::EnterReady()
{
    CurrentAttack = FCommittedAttack();
    SetState(ECommittedCombatState::Ready);
}

bool UCommittedCombatComponent::StartReversal()
{
    if (State != ECommittedCombatState::Ready || !GetWorld())
    {
        return false;
    }

    SetState(ECommittedCombatState::Reversal);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UCommittedCombatComponent::EnterReversalRecovery, ReversalWindowSeconds, false);
    return true;
}

void UCommittedCombatComponent::EnterReversalRecovery()
{
    SetState(ECommittedCombatState::Recovery);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UCommittedCombatComponent::EnterReady, ReversalRecoverySeconds, false);
}

void UCommittedCombatComponent::Stagger(float Duration)
{
    if (!GetWorld() || State == ECommittedCombatState::Defeated)
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    SetState(ECommittedCombatState::Staggered);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UCommittedCombatComponent::EnterReady, Duration, false);
}

void UCommittedCombatComponent::ReceiveAttack(UCommittedCombatComponent* Attacker,
    const FCommittedAttack& Attack)
{
    if (!Attacker || State == ECommittedCombatState::Defeated)
    {
        return;
    }

    if (State == ECommittedCombatState::Reversal)
    {
        const FVector ToAttacker = Attacker->GetOwner()->GetActorLocation() - GetOwner()->GetActorLocation();
        const float Facing = FVector::DotProduct(GetOwner()->GetActorForwardVector().GetSafeNormal2D(),
            ToAttacker.GetSafeNormal2D());
        if (Facing >= FMath::Cos(FMath::DegreesToRadians(70.f)))
        {
            GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
            EnterReady();
            Attacker->Stagger(ReversedAttackerStaggerSeconds);
            return;
        }
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    }
    Health = Attack.bLethal ? 0.f : FMath::Max(0.f, Health - Attack.Damage);
    if (Attack.TargetLimb != ECommittedLimb::None)
    {
        ImpairedLimbs.AddUnique(Attack.TargetLimb);
    }
    OnHitReceived.Broadcast(Attack.TargetLimb, Attack.bLethal);

    if (Health <= 0.f)
    {
        SetState(ECommittedCombatState::Defeated);
    }
    else
    {
        Stagger(0.35f);
    }
}

void UCommittedCombatComponent::ScanForTargets()
{
    if (State != ECommittedCombatState::Active || !GetWorld() || !GetOwner())
    {
        return;
    }

    TArray<FOverlapResult> Results;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(CommittedCombat), false, GetOwner());
    const FVector Origin = GetOwner()->GetActorLocation();
    GetWorld()->OverlapMultiByObjectType(Results, Origin, FQuat::Identity,
        FCollisionObjectQueryParams(ECC_Pawn),
        FCollisionShape::MakeSphere(CurrentAttack.Range), Params);

    for (const FOverlapResult& Result : Results)
    {
        AActor* Other = Result.GetActor();
        if (!Other || Other == GetOwner())
        {
            continue;
        }

        UCommittedCombatComponent* Target = Other->FindComponentByClass<UCommittedCombatComponent>();
        if (!Target || HitThisAttack.Contains(Target) || Target->State == ECommittedCombatState::Defeated)
        {
            continue;
        }

        const FVector ToTarget = Other->GetActorLocation() - Origin;
        const FVector FlatDirection = FVector(ToTarget.X, ToTarget.Y, 0.f).GetSafeNormal();
        const float Dot = FVector::DotProduct(GetOwner()->GetActorForwardVector(), FlatDirection);
        if (Dot < FMath::Cos(FMath::DegreesToRadians(CurrentAttack.HalfAngleDegrees)))
        {
            continue;
        }

        HitThisAttack.Add(Target);
        Target->ReceiveAttack(this, CurrentAttack);
        if (State != ECommittedCombatState::Active)
        {
            break; // A successful reversal interrupted this attack.
        }
    }
}

void UCommittedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ScanForTargets();
}

void UCommittedCombatComponent::ResetCombat()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    }
    Health = MaxHealth;
    ImpairedLimbs.Reset();
    HitThisAttack.Reset();
    EnterReady();
}
