#include "DevotedCombatComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionQueryParams.h"
#include "TimerManager.h"

UDevotedCombatComponent::UDevotedCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UDevotedCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
}

void UDevotedCombatComponent::SetState(EDevotedCombatState NewState)
{
    State = NewState;
    OnCombatStateChanged.Broadcast(State, CurrentAttack.Name);
}

bool UDevotedCombatComponent::StartAttack(const FDevotedAttack& Attack)
{
    if (State != EDevotedCombatState::Ready || !GetWorld() ||
        Attack.StartupSeconds <= 0.f || Attack.ActiveSeconds <= 0.f ||
        Attack.RecoverySeconds <= 0.f || Attack.Range <= 0.f)
    {
        return false;
    }

    CurrentAttack = Attack;
    HitThisAttack.Reset();
    SetState(EDevotedCombatState::Startup);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UDevotedCombatComponent::EnterActive, Attack.StartupSeconds, false);
    return true;
}

void UDevotedCombatComponent::EnterActive()
{
    SetState(EDevotedCombatState::Active);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UDevotedCombatComponent::EnterRecovery, CurrentAttack.ActiveSeconds, false);
    ScanForTargets();
}

void UDevotedCombatComponent::EnterRecovery()
{
    SetState(EDevotedCombatState::Recovery);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UDevotedCombatComponent::EnterReady, CurrentAttack.RecoverySeconds, false);
}

void UDevotedCombatComponent::EnterReady()
{
    CurrentAttack = FDevotedAttack();
    SetState(EDevotedCombatState::Ready);
}

bool UDevotedCombatComponent::StartReversal()
{
    if (State != EDevotedCombatState::Ready || !GetWorld())
    {
        return false;
    }

    SetState(EDevotedCombatState::Reversal);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UDevotedCombatComponent::EnterReversalRecovery, ReversalWindowSeconds, false);
    return true;
}

void UDevotedCombatComponent::EnterReversalRecovery()
{
    SetState(EDevotedCombatState::Recovery);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UDevotedCombatComponent::EnterReady, ReversalRecoverySeconds, false);
}

void UDevotedCombatComponent::Stagger(float Duration)
{
    if (!GetWorld() || State == EDevotedCombatState::Defeated)
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    SetState(EDevotedCombatState::Staggered);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this,
        &UDevotedCombatComponent::EnterReady, Duration, false);
}

void UDevotedCombatComponent::ReceiveAttack(UDevotedCombatComponent* Attacker,
    const FDevotedAttack& Attack)
{
    if (!Attacker || State == EDevotedCombatState::Defeated)
    {
        return;
    }

    if (State == EDevotedCombatState::Reversal)
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
    if (Attack.TargetLimb != EDevotedLimb::None)
    {
        ImpairedLimbs.AddUnique(Attack.TargetLimb);
    }
    OnHitReceived.Broadcast(Attack.TargetLimb, Attack.bLethal);

    if (Health <= 0.f)
    {
        SetState(EDevotedCombatState::Defeated);
    }
    else
    {
        Stagger(0.35f);
    }
}

void UDevotedCombatComponent::ScanForTargets()
{
    if (State != EDevotedCombatState::Active || !GetWorld() || !GetOwner())
    {
        return;
    }

    TArray<FOverlapResult> Results;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(DevotedCombat), false, GetOwner());
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

        UDevotedCombatComponent* Target = Other->FindComponentByClass<UDevotedCombatComponent>();
        if (!Target || HitThisAttack.Contains(Target) || Target->State == EDevotedCombatState::Defeated)
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
        if (State != EDevotedCombatState::Active)
        {
            break; // A successful reversal interrupted this attack.
        }
    }
}

void UDevotedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ScanForTargets();
}

void UDevotedCombatComponent::ResetCombat()
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
