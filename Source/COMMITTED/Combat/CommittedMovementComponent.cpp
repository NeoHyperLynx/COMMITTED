#include "CommittedMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

UCommittedMovementComponent::UCommittedMovementComponent()
{
    MaxWalkSpeed = NormalWalkSpeed;
    MaxAcceleration = 2400.f;
    BrakingDecelerationWalking = 2000.f;
    GroundFriction = 8.f;
    JumpZVelocity = 580.f;
    AirControl = 0.5f;
    GravityScale = 1.6f;
    AvailableDashCharges = MaxDashCharges;
}

void UCommittedMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
    if (CharacterOwner && CharacterOwner->GetCapsuleComponent())
    {
        DefaultCapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
    }
}

void UCommittedMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Recharge dash charges
    if (AvailableDashCharges < MaxDashCharges)
    {
        AvailableDashCharges = FMath::Min(static_cast<float>(MaxDashCharges), AvailableDashCharges + DashChargeRechargeRate * DeltaTime);
    }

    if (DashCooldownRemaining > 0.f)
    {
        DashCooldownRemaining = FMath::Max(0.f, DashCooldownRemaining - DeltaTime);
    }

    // Reset air jumps on ground
    if (IsMovingOnGround())
    {
        CurrentAirJumpCount = 0;
    }
}

void UCommittedMovementComponent::SetWantsToSprint(bool bSprint)
{
    bWantsToSprint = bSprint;
}

bool UCommittedMovementComponent::IsCustomMovementMode(ECommittedCustomMovementMode InCustomMode) const
{
    return MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(InCustomMode);
}

ECommittedCustomMovementMode UCommittedMovementComponent::GetCommittedCustomMovementMode() const
{
    if (MovementMode == MOVE_Custom)
    {
        return static_cast<ECommittedCustomMovementMode>(CustomMovementMode);
    }
    return ECommittedCustomMovementMode::None;
}

void UCommittedMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
    // Auto-detect wall run if falling and moving towards/along a wall
    if (IsFalling() && !IsCustomMovementMode(ECommittedCustomMovementMode::Dash) && !IsCustomMovementMode(ECommittedCustomMovementMode::WallRun))
    {
        if (Velocity.Z < 100.f && Velocity.Size2D() > 200.f)
        {
            TryWallRun();
        }
    }

    Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

bool UCommittedMovementComponent::StartSlide()
{
    if (!IsMovingOnGround() || IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
    {
        return false;
    }

    FVector MoveDir = Velocity.GetSafeNormal2D();
    if (MoveDir.IsNearlyZero())
    {
        if (CharacterOwner)
        {
            MoveDir = CharacterOwner->GetActorForwardVector();
        }
        else
        {
            return false;
        }
    }

    SlideDirection = MoveDir;
    SlideTimeRemaining = SlideMaxDuration;
    bIsSliding = true;

    // Boost speed into slide
    const float CurrentSpd = Velocity.Size2D();
    const float TargetSpd = FMath::Max(CurrentSpd, SlideInitialSpeed);
    Velocity = SlideDirection * TargetSpd;

    // Crouch capsule
    if (CharacterOwner && CharacterOwner->GetCapsuleComponent())
    {
        CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(SlideCapsuleHalfHeight, true);
    }

    SetMovementMode(MOVE_Custom, static_cast<uint8>(ECommittedCustomMovementMode::Slide));
    return true;
}

void UCommittedMovementComponent::StopSlide()
{
    if (!bIsSliding && !IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
    {
        return;
    }

    bIsSliding = false;
    SlideTimeRemaining = 0.f;

    // Restore capsule height
    if (CharacterOwner && CharacterOwner->GetCapsuleComponent())
    {
        CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight, true);
    }

    if (IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
    {
        SetMovementMode(MOVE_Walking);
    }
}

bool UCommittedMovementComponent::StartDash(FVector WorldDirection)
{
    if (AvailableDashCharges < 1.f || DashCooldownRemaining > 0.f)
    {
        return false;
    }

    // Cancel slide if sliding
    if (bIsSliding)
    {
        StopSlide();
    }

    FVector Direction = WorldDirection;
    if (Direction.IsNearlyZero())
    {
        Direction = Velocity.GetSafeNormal2D();
        if (Direction.IsNearlyZero() && CharacterOwner)
        {
            Direction = CharacterOwner->GetActorForwardVector();
        }
    }
    Direction.Z = 0.f;
    Direction.Normalize();

    AvailableDashCharges -= 1.f;
    DashCooldownRemaining = DashCooldown;
    DashTimeRemaining = DashDuration;
    bIsDashing = true;
    DashVelocityVector = Direction * DashSpeed;

    Velocity = DashVelocityVector;

    SetMovementMode(MOVE_Custom, static_cast<uint8>(ECommittedCustomMovementMode::Dash));
    return true;
}

bool UCommittedMovementComponent::CheckWallRunSurfaces(FHitResult& OutHit, ECommittedWallRunSide& OutSide) const
{
    if (!CharacterOwner || !GetWorld()) return false;

    const FVector ActorLoc = CharacterOwner->GetActorLocation();
    const FVector RightVec = CharacterOwner->GetActorRightVector();

    FCollisionQueryParams QueryParams(TEXT("WallRunTrace"), false, CharacterOwner);

    // Trace Right
    FHitResult RightHit;
    const FVector RightEnd = ActorLoc + (RightVec * WallRunLineTraceLength);
    if (GetWorld()->LineTraceSingleByChannel(RightHit, ActorLoc, RightEnd, ECC_WorldStatic, QueryParams))
    {
        if (FMath::Abs(RightHit.Normal.Z) < 0.2f) // reasonably vertical wall
        {
            OutHit = RightHit;
            OutSide = ECommittedWallRunSide::Right;
            return true;
        }
    }

    // Trace Left
    FHitResult LeftHit;
    const FVector LeftEnd = ActorLoc - (RightVec * WallRunLineTraceLength);
    if (GetWorld()->LineTraceSingleByChannel(LeftHit, ActorLoc, LeftEnd, ECC_WorldStatic, QueryParams))
    {
        if (FMath::Abs(LeftHit.Normal.Z) < 0.2f)
        {
            OutHit = LeftHit;
            OutSide = ECommittedWallRunSide::Left;
            return true;
        }
    }

    return false;
}

bool UCommittedMovementComponent::TryWallRun()
{
    if (IsMovingOnGround()) return false;

    FHitResult WallHit;
    ECommittedWallRunSide Side = ECommittedWallRunSide::None;
    if (!CheckWallRunSurfaces(WallHit, Side))
    {
        return false;
    }

    WallNormal = WallHit.Normal;
    CurrentWallRunSide = Side;

    // Calculate wall forward direction
    // Wall run forward should be tangent to the wall in character's forward direction
    const FVector WallTangent = (Side == ECommittedWallRunSide::Right)
        ? FVector::CrossProduct(WallNormal, FVector::UpVector)
        : FVector::CrossProduct(FVector::UpVector, WallNormal);

    WallRunDirection = WallTangent.GetSafeNormal();
    WallRunTimeRemaining = WallRunMaxDuration;
    bIsWallRunning = true;

    Velocity = WallRunDirection * WallRunSpeed;

    SetMovementMode(MOVE_Custom, static_cast<uint8>(ECommittedCustomMovementMode::WallRun));
    return true;
}

bool UCommittedMovementComponent::TryWallKick()
{
    if (!bIsWallRunning && !IsCustomMovementMode(ECommittedCustomMovementMode::WallRun))
    {
        // Also check if near a wall while airborne
        FHitResult WallHit;
        ECommittedWallRunSide Side;
        if (IsFalling() && CheckWallRunSurfaces(WallHit, Side))
        {
            WallNormal = WallHit.Normal;
        }
        else
        {
            return false;
        }
    }

    bIsWallRunning = false;
    CurrentWallRunSide = ECommittedWallRunSide::None;

    // Jump away from wall and slightly forward
    FVector KickDir = WallNormal + (CharacterOwner ? CharacterOwner->GetActorForwardVector() * 0.4f : FVector::ZeroVector);
    KickDir.Z = 0.f;
    KickDir.Normalize();

    const FVector KickVelocity = (KickDir * WallKickImpulse.X) + FVector(0.f, 0.f, WallKickImpulse.Z);
    Velocity = KickVelocity;

    SetMovementMode(MOVE_Falling);
    return true;
}

bool UCommittedMovementComponent::TryAirJump()
{
    if (IsMovingOnGround() || CurrentAirJumpCount >= MaxAirJumps)
    {
        return false;
    }

    CurrentAirJumpCount++;
    Velocity.Z = JumpZVelocity * 0.9f;
    SetMovementMode(MOVE_Falling);
    return true;
}

bool UCommittedMovementComponent::DoJump(bool bReplayingMoves)
{
    if (IsCustomMovementMode(ECommittedCustomMovementMode::WallRun))
    {
        return TryWallKick();
    }

    if (IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
    {
        StopSlide();
        return Super::DoJump(bReplayingMoves);
    }

    if (IsFalling())
    {
        if (TryWallKick())
        {
            return true;
        }
        if (TryAirJump())
        {
            return true;
        }
        return false;
    }

    return Super::DoJump(bReplayingMoves);
}

void UCommittedMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    if (PreviousMovementMode == MOVE_Custom)
    {
        if (PreviousCustomMode == static_cast<uint8>(ECommittedCustomMovementMode::Slide))
        {
            bIsSliding = false;
            if (CharacterOwner && CharacterOwner->GetCapsuleComponent())
            {
                CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight, true);
            }
        }
        else if (PreviousCustomMode == static_cast<uint8>(ECommittedCustomMovementMode::Dash))
        {
            bIsDashing = false;
        }
        else if (PreviousCustomMode == static_cast<uint8>(ECommittedCustomMovementMode::WallRun))
        {
            bIsWallRunning = false;
            CurrentWallRunSide = ECommittedWallRunSide::None;
        }
    }

    ECommittedCustomMovementMode NewCustom = GetCommittedCustomMovementMode();
    OnCustomMovementModeChanged.Broadcast(NewCustom);
}

void UCommittedMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
    Super::PhysCustom(deltaTime, Iterations);

    switch (static_cast<ECommittedCustomMovementMode>(CustomMovementMode))
    {
    case ECommittedCustomMovementMode::Slide:
        PhysSlide(deltaTime, Iterations);
        break;
    case ECommittedCustomMovementMode::WallRun:
        PhysWallRun(deltaTime, Iterations);
        break;
    case ECommittedCustomMovementMode::Dash:
        PhysDash(deltaTime, Iterations);
        break;
    default:
        SetMovementMode(MOVE_Walking);
        break;
    }
}

void UCommittedMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME) return;

    SlideTimeRemaining -= deltaTime;
    if (SlideTimeRemaining <= 0.f || Velocity.Size2D() < SlideMinSpeed)
    {
        StopSlide();
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    // Apply friction to decay slide speed
    const float Decel = SlideFriction * 980.f * deltaTime;
    const float NewSpeed = FMath::Max(0.f, Velocity.Size2D() - Decel);
    Velocity = SlideDirection * NewSpeed;

    // Move along floor
    FHitResult Hit;
    FVector Delta = Velocity * deltaTime;
    SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
    }

    // Ensure still on ground
    if (!IsMovingOnGround())
    {
        StopSlide();
        SetMovementMode(MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
    }
}

void UCommittedMovementComponent::PhysWallRun(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME) return;

    WallRunTimeRemaining -= deltaTime;

    FHitResult WallHit;
    ECommittedWallRunSide Side;
    if (WallRunTimeRemaining <= 0.f || !CheckWallRunSurfaces(WallHit, Side) || Side != CurrentWallRunSide)
    {
        bIsWallRunning = false;
        CurrentWallRunSide = ECommittedWallRunSide::None;
        SetMovementMode(MOVE_Falling);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    WallNormal = WallHit.Normal;
    const FVector WallTangent = (Side == ECommittedWallRunSide::Right)
        ? FVector::CrossProduct(WallNormal, FVector::UpVector)
        : FVector::CrossProduct(FVector::UpVector, WallNormal);
    WallRunDirection = WallTangent.GetSafeNormal();

    // Gentle vertical sag with reduced gravity
    Velocity = (WallRunDirection * WallRunSpeed) + FVector(0.f, 0.f, -980.f * WallRunGravityScale * (WallRunMaxDuration - WallRunTimeRemaining));

    FHitResult MoveHit;
    FVector Delta = Velocity * deltaTime;
    SafeMoveUpdatedComponent(Delta, WallRunDirection.Rotation(), true, MoveHit);

    if (MoveHit.IsValidBlockingHit())
    {
        SlideAlongSurface(Delta, 1.f - MoveHit.Time, MoveHit.Normal, MoveHit, true);
    }
}

void UCommittedMovementComponent::PhysDash(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME) return;

    DashTimeRemaining -= deltaTime;
    if (DashTimeRemaining <= 0.f)
    {
        bIsDashing = false;
        SetMovementMode(MOVE_Walking);
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    Velocity = DashVelocityVector;
    FHitResult Hit;
    FVector Delta = Velocity * deltaTime;
    SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
    }
}
