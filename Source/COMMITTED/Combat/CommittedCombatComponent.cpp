#include "CommittedCombatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

UCommittedCombatComponent::UCommittedCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    Health = MaxHealth;
    Posture = 0.f;
    Flow = 0.f;
    State = ECommittedCombatState::Ready;
}

void UCommittedCombatComponent::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
    Posture = 0.f;
    Flow = 0.f;
}

void UCommittedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Deflect active window count down
    if (State == ECommittedCombatState::Deflect)
    {
        DeflectActiveTime -= DeltaTime;
        if (DeflectActiveTime <= 0.f)
        {
            EnterDeflectRecovery();
        }
    }

    // Posture regeneration logic
    if (PostureRegenTimer > 0.f)
    {
        PostureRegenTimer -= DeltaTime;
    }
    else if (Posture > 0.f && State != ECommittedCombatState::PostureBroken)
    {
        float CurrentRate = PostureRegenRate;
        if (Health < (MaxHealth * 0.5f))
        {
            CurrentRate *= 0.5f; // low health slows posture recovery
        }
        Posture = FMath::Max(0.f, Posture - (CurrentRate * DeltaTime));
    }

    // Flow decay logic
    if (FlowDecayTimer > 0.f)
    {
        FlowDecayTimer -= DeltaTime;
    }
    else if (Flow > 0.f)
    {
        Flow = FMath::Max(0.f, Flow - (FlowDecayRate * DeltaTime));
    }

    // Grab duration timeout
    if (State == ECommittedCombatState::Grabbed)
    {
        GrabTimer -= DeltaTime;
        if (GrabTimer <= 0.f)
        {
            BreakGrab();
        }
    }

    // Consecutive deflect chain timer (decays if no deflect within 0.8s)
    if (ConsecutiveDeflectResetTimer > 0.f)
    {
        ConsecutiveDeflectResetTimer -= DeltaTime;
        if (ConsecutiveDeflectResetTimer <= 0.f)
        {
            ConsecutiveDeflects = 0;
        }
    }

    // Periodic environmental surface scan
    ScanEnvironmentalSurfaces(200.f);
}

void UCommittedCombatComponent::SetState(ECommittedCombatState NewState)
{
    State = NewState;
    OnCombatStateChanged.Broadcast(State, CurrentAttack.Name);
}

void UCommittedCombatComponent::AddPosture(float Amount)
{
    Posture = FMath::Clamp(Posture + Amount, 0.f, MaxPosture);
    PostureRegenTimer = PostureRegenDelay;

    if (Posture >= MaxPosture && State != ECommittedCombatState::PostureBroken && State != ECommittedCombatState::Defeated)
    {
        EnterPostureBroken();
    }
}

void UCommittedCombatComponent::AddFlow(float Amount)
{
    Flow = FMath::Clamp(Flow + Amount, 0.f, MaxFlow);
    FlowDecayTimer = FlowDecayDelay;
}

void UCommittedCombatComponent::EnterPostureBroken()
{
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    SetState(ECommittedCombatState::PostureBroken);

    // If owner is player, apply 1.4s player break duration; enemies receive 2.0s vulnerability
    AActor* OwnerActor = GetOwner();
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
    const bool bIsPlayer = (OwnerActor && OwnerActor == PlayerChar);

    const float BreakDuration = bIsPlayer ? 1.40f : 2.0f;
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::RecoverFromPostureBroken, BreakDuration, false);
}

void UCommittedCombatComponent::RecoverFromPostureBroken()
{
    if (State == ECommittedCombatState::PostureBroken)
    {
        // Player recovers with 60 posture; enemies recover with 70
        AActor* OwnerActor = GetOwner();
        ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
        const bool bIsPlayer = (OwnerActor && OwnerActor == PlayerChar);

        Posture = MaxPosture * (bIsPlayer ? 0.60f : 0.70f);
        EnterReady();
    }
}

bool UCommittedCombatComponent::StartAttack(const FCommittedAttack& Attack)
{
    if (State != ECommittedCombatState::Ready && State != ECommittedCombatState::Recovery)
    {
        return false;
    }

    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    HitThisAttack.Empty();
    CurrentAttack = Attack;
    SetState(ECommittedCombatState::Startup);

    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterActive,
        FMath::Max(0.01f, Attack.StartupSeconds), false);
    return true;
}

void UCommittedCombatComponent::EnterActive()
{
    SetState(ECommittedCombatState::Active);
    ScanForTargets();
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterRecovery,
        FMath::Max(0.01f, CurrentAttack.ActiveSeconds), false);
}

void UCommittedCombatComponent::EnterRecovery()
{
    SetState(ECommittedCombatState::Recovery);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterReady,
        FMath::Max(0.01f, CurrentAttack.RecoverySeconds), false);
}

void UCommittedCombatComponent::EnterReady()
{
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    CurrentAttack = FCommittedAttack();
    SetState(ECommittedCombatState::Ready);
}

bool UCommittedCombatComponent::StartDeflect()
{
    if (State != ECommittedCombatState::Ready) return false;

    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    CurrentAttack = FCommittedAttack();
    CurrentAttack.Name = TEXT("Deflect Window");
    DeflectActiveTime = PerfectDeflectWindowSeconds;
    SetState(ECommittedCombatState::Deflect);
    return true;
}

void UCommittedCombatComponent::EnterDeflectRecovery()
{
    SetState(ECommittedCombatState::Recovery);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterReady,
        FMath::Max(0.01f, DeflectRecoverySeconds), false);
}

void UCommittedCombatComponent::ScanForTargets()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !GetWorld()) return;

    TArray<AActor*> Fighters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Fighters);

    const FVector Forward = OwnerActor->GetActorForwardVector();
    const FVector Origin = OwnerActor->GetActorLocation();

    for (AActor* OtherActor : Fighters)
    {
        if (OtherActor == OwnerActor) continue;
        UCommittedCombatComponent* OtherCombat = OtherActor->FindComponentByClass<UCommittedCombatComponent>();
        if (!OtherCombat || HitThisAttack.Contains(OtherCombat)) continue;

        const FVector ToOther = OtherActor->GetActorLocation() - Origin;
        if (ToOther.Size() > CurrentAttack.Range) continue;

        const float FacingDot = FVector::DotProduct(Forward, ToOther.GetSafeNormal());
        const float MinDot = FMath::Cos(FMath::DegreesToRadians(CurrentAttack.HalfAngleDegrees));
        if (FacingDot < MinDot) continue;

        HitThisAttack.Add(OtherCombat);
        OtherCombat->ReceiveAttack(this, CurrentAttack);
    }
}

void UCommittedCombatComponent::ReceiveAttack(UCommittedCombatComponent* Attacker, const FCommittedAttack& Attack)
{
    if (State == ECommittedCombatState::Defeated) return;

    AActor* DefenderActor = GetOwner();
    AActor* AttackerActor = Attacker ? Attacker->GetOwner() : nullptr;

    // Check Deflect: front angle during Deflect state
    if (State == ECommittedCombatState::Deflect && DefenderActor && AttackerActor && !Attack.bUnblockable)
    {
        const FVector ToAttacker = (AttackerActor->GetActorLocation() - DefenderActor->GetActorLocation()).GetSafeNormal();
        const float FacingDot = FVector::DotProduct(DefenderActor->GetActorForwardVector(), ToAttacker);

        if (FacingDot > 0.35f)
        {
            DeflectAttacker(Attacker);
            return;
        }
    }

    // Hit connected
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);

    if (Attack.TargetLimb != ECommittedLimb::None)
    {
        ImpairedLimbs.AddUnique(Attack.TargetLimb);
    }

    float DamageDealt = Attack.bLethal ? Health : Attack.Damage;
    if (State == ECommittedCombatState::PostureBroken)
    {
        DamageDealt *= 1.75f; // 1.75x unmitigated damage during posture break
    }
    Health = FMath::Max(0.f, Health - DamageDealt);

    AddPosture(Attack.PostureDamage);
    if (Attacker) Attacker->AddFlow(Attacker->FlowGainRate);

    OnHitReceived.Broadcast(Attack.TargetLimb, Attack.bLethal);

    if (Health <= 0.f || Attack.bLethal)
    {
        SetState(ECommittedCombatState::Defeated);
        return;
    }

    Stagger(0.45f);
}

void UCommittedCombatComponent::DeflectAttacker(UCommittedCombatComponent* Attacker)
{
    if (!Attacker) return;

    ConsecutiveDeflects++;
    ConsecutiveDeflectResetTimer = 0.80f; // 0.8s chain window as specified in design doc
    const float Multiplier = FMath::Clamp(1.f + (ConsecutiveDeflects - 1) * 0.25f, 1.f, 2.5f);

    // Inflict posture damage on attacker using exact formula: 24.0 * Multiplier
    Attacker->AddPosture(24.f * Multiplier);
    AddFlow(18.f);

    // Attacker staggered
    Attacker->Stagger(AttackerDeflectedStaggerSeconds);

    // Cancel own recovery and enter ready immediately for follow-up
    EnterReady();
}

void UCommittedCombatComponent::Stagger(float Duration)
{
    SetState(ECommittedCombatState::Staggered);
    GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterReady,
        FMath::Max(0.05f, Duration), false);
}

bool UCommittedCombatComponent::ScanEnvironmentalSurfaces(float MaxRadius)
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !GetWorld()) return false;

    const FVector Loc = OwnerActor->GetActorLocation();
    FCollisionQueryParams Params(TEXT("EnvScan"), false, OwnerActor);

    // Check downward for cliff / ledge edge
    FHitResult DownHit;
    if (GetWorld()->LineTraceSingleByChannel(DownHit, Loc, Loc - FVector(0.f, 0.f, 400.f), ECC_WorldStatic, Params))
    {
        // Check 1.5m ahead of character to see if floor drops off (ledge)
        const FVector ForwardLoc = Loc + OwnerActor->GetActorForwardVector() * 150.f;
        FHitResult LedgeHit;
        if (!GetWorld()->LineTraceSingleByChannel(LedgeHit, ForwardLoc, ForwardLoc - FVector(0.f, 0.f, 500.f), ECC_WorldStatic, Params))
        {
            NearbyEnvironmentalSurface = EEnvironmentalSurfaceType::Ledge;
            NearbySurfacePoint = ForwardLoc;
            NearbySurfaceNormal = -OwnerActor->GetActorForwardVector();
            return true;
        }
    }

    // Trace in 8 radial directions for walls or pillars
    for (int32 i = 0; i < 8; ++i)
    {
        const float Angle = i * (360.f / 8.f);
        const FVector Dir = FRotator(0.f, Angle, 0.f).RotateVector(FVector::ForwardVector);
        FHitResult WallHit;
        if (GetWorld()->LineTraceSingleByChannel(WallHit, Loc, Loc + (Dir * MaxRadius), ECC_WorldStatic, Params))
        {
            NearbySurfacePoint = WallHit.ImpactPoint;
            NearbySurfaceNormal = WallHit.ImpactNormal;

            if (WallHit.GetActor() && WallHit.GetActor()->GetName().Contains(TEXT("Pillar")))
            {
                NearbyEnvironmentalSurface = EEnvironmentalSurfaceType::Pillar;
            }
            else
            {
                NearbyEnvironmentalSurface = EEnvironmentalSurfaceType::Wall;
            }
            return true;
        }
    }

    NearbyEnvironmentalSurface = EEnvironmentalSurfaceType::None;
    return false;
}

bool UCommittedCombatComponent::TryEnvironmentalTakedown()
{
    if (NearbyEnvironmentalSurface == EEnvironmentalSurfaceType::None) return false;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !GetWorld()) return false;

    // 1. Check if holding a grabbed opponent for a Grab -> Environmental Chain
    if (GrabbedOpponent.IsValid())
    {
        UCommittedCombatComponent* TargetCombat = GrabbedOpponent.Get();
        SetState(ECommittedCombatState::PerformingTakedown);
        OnEnvironmentalTakedown.Broadcast(NearbyEnvironmentalSurface, TargetCombat->GetOwner());

        // Grab-chain slam inflicts 50% max HP damage
        TargetCombat->Health = FMath::Max(0.f, TargetCombat->Health - (TargetCombat->MaxHealth * 0.50f));
        if (TargetCombat->Health <= 0.f)
        {
            TargetCombat->SetState(ECommittedCombatState::Defeated);
        }
        else
        {
            TargetCombat->Stagger(1.5f);
        }

        BreakGrab();
        AddFlow(25.f);

        GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterReady, 0.85f, false);
        return true;
    }

    // 2. Check for Posture Broken opponent within 2.0m (200 units)
    if (State != ECommittedCombatState::Ready) return false;

    TArray<AActor*> Fighters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Fighters);

    for (AActor* OtherActor : Fighters)
    {
        if (OtherActor == OwnerActor) continue;
        UCommittedCombatComponent* OtherCombat = OtherActor->FindComponentByClass<UCommittedCombatComponent>();
        if (!OtherCombat) continue;

        const float Dist = FVector::Dist(OwnerActor->GetActorLocation(), OtherActor->GetActorLocation());
        if (Dist <= 200.f && OtherCombat->State == ECommittedCombatState::PostureBroken)
        {
            SetState(ECommittedCombatState::PerformingTakedown);
            OnEnvironmentalTakedown.Broadcast(NearbyEnvironmentalSurface, OtherActor);

            // Execute takedown
            if (NearbyEnvironmentalSurface == EEnvironmentalSurfaceType::Ledge)
            {
                // Instant ejection / defeat
                OtherCombat->Health = 0.f;
                OtherCombat->SetState(ECommittedCombatState::Defeated);
            }
            else
            {
                // Wall or Pillar slam
                OtherCombat->Health = FMath::Max(0.f, OtherCombat->Health - (OtherCombat->MaxHealth * 0.75f));
                if (OtherCombat->Health <= 0.f)
                {
                    OtherCombat->SetState(ECommittedCombatState::Defeated);
                }
                else
                {
                    OtherCombat->Stagger(1.5f);
                }
            }

            AddFlow(35.f);

            // 0.85s invulnerable execution window before returning to ready
            GetWorld()->GetTimerManager().SetTimer(PhaseTimer, this, &UCommittedCombatComponent::EnterReady, 0.85f, false);
            return true;
        }
    }

    return false;
}

bool UCommittedCombatComponent::TryDedicatedGrab(EGrabTargetPart TargetPart)
{
    if (State != ECommittedCombatState::Ready) return false;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !GetWorld()) return false;

    TArray<AActor*> Fighters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Fighters);

    for (AActor* OtherActor : Fighters)
    {
        if (OtherActor == OwnerActor) continue;
        UCommittedCombatComponent* OtherCombat = OtherActor->FindComponentByClass<UCommittedCombatComponent>();
        if (!OtherCombat) continue;

        const float Dist = FVector::Dist(OwnerActor->GetActorLocation(), OtherActor->GetActorLocation());
        if (Dist <= 180.f && OtherCombat->State != ECommittedCombatState::Defeated)
        {
            GrabbedOpponent = OtherCombat;
            OtherCombat->GrabbedByOpponent = this;
            OtherCombat->SetState(ECommittedCombatState::Grabbed);
            OtherCombat->GrabTimer = 1.8f;

            AddPosture(10.f); // initial grab leverage
            return true;
        }
    }

    return false;
}

bool UCommittedCombatComponent::TryGrabThrow(FVector ThrowDirection)
{
    if (!GrabbedOpponent.IsValid()) return false;

    UCommittedCombatComponent* TargetCombat = GrabbedOpponent.Get();
    ACharacter* TargetChar = Cast<ACharacter>(TargetCombat->GetOwner());

    if (TargetChar && TargetChar->GetCharacterMovement())
    {
        FVector Dir = ThrowDirection.GetSafeNormal();
        if (Dir.IsNearlyZero() && GetOwner())
        {
            Dir = GetOwner()->GetActorForwardVector();
        }
        TargetChar->LaunchCharacter(Dir * 900.f + FVector(0.f, 0.f, 250.f), true, true);
    }

    BreakGrab();
    TargetCombat->Stagger(1.2f);
    return true;
}

void UCommittedCombatComponent::BreakGrab()
{
    if (GrabbedOpponent.IsValid())
    {
        GrabbedOpponent->GrabbedByOpponent = nullptr;
        if (GrabbedOpponent->State == ECommittedCombatState::Grabbed)
        {
            GrabbedOpponent->EnterReady();
        }
        GrabbedOpponent = nullptr;
    }

    if (GrabbedByOpponent.IsValid())
    {
        GrabbedByOpponent->GrabbedOpponent = nullptr;
        GrabbedByOpponent = nullptr;
    }

    if (State == ECommittedCombatState::Grabbed)
    {
        EnterReady();
    }
}

void UCommittedCombatComponent::ResetCombat()
{
    GetWorld()->GetTimerManager().ClearTimer(PhaseTimer);
    Health = MaxHealth;
    Posture = 0.f;
    Flow = 0.f;
    ConsecutiveDeflects = 0;
    ConsecutiveDeflectResetTimer = 0.f;
    ImpairedLimbs.Empty();
    HitThisAttack.Empty();
    BreakGrab();
    EnterReady();
}
