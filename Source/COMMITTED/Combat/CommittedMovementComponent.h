#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CommittedMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECommittedCustomMovementMode : uint8
{
    None = 0,
    Slide,
    WallRun,
    Dash
};

UENUM(BlueprintType)
enum class ECommittedWallRunSide : uint8
{
    None,
    Left,
    Right
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCommittedMovementModeChangedDelegate, ECommittedCustomMovementMode, NewCustomMode);

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class COMMITTED_API UCommittedMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UCommittedMovementComponent();

    UPROPERTY(BlueprintAssignable, Category="COMMITTED|Movement")
    FCommittedMovementModeChangedDelegate OnCustomMovementModeChanged;

    // --- Speeds & Tunables ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Sprint", meta=(ClampMin="100.0"))
    float NormalWalkSpeed = 480.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Sprint", meta=(ClampMin="100.0"))
    float SprintSpeed = 820.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Sprint", meta=(ClampMin="50.0"))
    float ImpairedWalkSpeed = 280.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Sprint", meta=(ClampMin="50.0"))
    float ImpairedSprintSpeed = 440.f;

    // --- Slide Tunables ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Slide", meta=(ClampMin="100.0"))
    float SlideInitialSpeed = 950.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Slide", meta=(ClampMin="0.0"))
    float SlideMinSpeed = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Slide", meta=(ClampMin="0.0"))
    float SlideFriction = 1.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Slide", meta=(ClampMin="0.1"))
    float SlideMaxDuration = 1.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Slide")
    float SlideCapsuleHalfHeight = 44.f;

    // --- Dash / Phantom Evade Tunables ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Dash", meta=(ClampMin="100.0"))
    float DashSpeed = 1600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Dash", meta=(ClampMin="0.05"))
    float DashDuration = 0.22f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Dash", meta=(ClampMin="0.0"))
    float DashCooldown = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Dash")
    int32 MaxDashCharges = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Dash")
    float DashChargeRechargeRate = 1.2f;

    // --- Wall Run & Kick Tunables ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|WallRun", meta=(ClampMin="100.0"))
    float WallRunSpeed = 750.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|WallRun", meta=(ClampMin="0.1"))
    float WallRunMaxDuration = 1.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|WallRun", meta=(ClampMin="0.0"))
    float WallRunGravityScale = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|WallRun")
    float WallRunLineTraceLength = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|WallRun")
    FVector WallKickImpulse = FVector(700.f, 0.f, 600.f);

    // --- Aerial Agility ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Movement|Aerial")
    int32 MaxAirJumps = 1;

    // --- State Queries ---
    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    bool bWantsToSprint = false;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    bool bIsDashing = false;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    bool bIsSliding = false;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    bool bIsWallRunning = false;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    ECommittedWallRunSide CurrentWallRunSide = ECommittedWallRunSide::None;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    FVector WallNormal = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Movement")
    float AvailableDashCharges = 2.f;

    // --- Activation Functions ---
    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    void SetWantsToSprint(bool bSprint);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    bool StartSlide();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    void StopSlide();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    bool StartDash(FVector WorldDirection = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    bool TryWallRun();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    bool TryWallKick();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Movement")
    bool TryAirJump();

    UFUNCTION(BlueprintPure, Category="COMMITTED|Movement")
    bool IsCustomMovementMode(ECommittedCustomMovementMode InCustomMode) const;

    UFUNCTION(BlueprintPure, Category="COMMITTED|Movement")
    ECommittedCustomMovementMode GetCommittedCustomMovementMode() const;

    virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
    virtual void PhysCustom(float deltaTime, int32 Iterations) override;
    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
    virtual bool DoJump(bool bReplayingMoves) override;

protected:
    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float DefaultCapsuleHalfHeight = 88.f;
    float SlideTimeRemaining = 0.f;
    FVector SlideDirection = FVector::ZeroVector;

    float DashTimeRemaining = 0.f;
    float DashCooldownRemaining = 0.f;
    FVector DashVelocityVector = FVector::ZeroVector;

    float WallRunTimeRemaining = 0.f;
    FVector WallRunDirection = FVector::ZeroVector;

    int32 CurrentAirJumpCount = 0;

    void PhysSlide(float deltaTime, int32 Iterations);
    void PhysWallRun(float deltaTime, int32 Iterations);
    void PhysDash(float deltaTime, int32 Iterations);

    bool CheckWallRunSurfaces(FHitResult& OutHit, ECommittedWallRunSide& OutSide) const;
};
