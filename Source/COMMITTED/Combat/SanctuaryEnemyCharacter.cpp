#include "SanctuaryEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ASanctuaryEnemyCharacter::ASanctuaryEnemyCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bPrototypeRivalAI = false; // We handle custom archetype logic in Tick
}

void ASanctuaryEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Archetype color tinting and attribute initialization
    FLinearColor ArchetypeColor = FLinearColor(0.8f, 0.2f, 0.2f);
    switch (Archetype)
    {
    case ESanctuaryEnemyArchetype::Grunt:
        ArchetypeColor = FLinearColor(0.85f, 0.4f, 0.1f);
        Combat->MaxHealth = 70.f;
        Combat->MaxPosture = 60.f;
        break;
    case ESanctuaryEnemyArchetype::Bruiser:
        ArchetypeColor = FLinearColor(0.45f, 0.1f, 0.1f);
        Combat->MaxHealth = 150.f;
        Combat->MaxPosture = 120.f;
        if (CommittedMovement) CommittedMovement->MaxWalkSpeed = 360.f;
        break;
    case ESanctuaryEnemyArchetype::Skirmisher:
        ArchetypeColor = FLinearColor(0.7f, 0.15f, 0.75f);
        Combat->MaxHealth = 80.f;
        Combat->MaxPosture = 70.f;
        if (CommittedMovement) CommittedMovement->MaxWalkSpeed = 620.f;
        break;
    case ESanctuaryEnemyArchetype::Scout:
        ArchetypeColor = FLinearColor(0.2f, 0.65f, 0.2f);
        Combat->MaxHealth = 60.f;
        Combat->MaxPosture = 50.f;
        break;
    case ESanctuaryEnemyArchetype::BossRival:
        ArchetypeColor = FLinearColor(0.95f, 0.08f, 0.08f);
        Combat->MaxHealth = 250.f;
        Combat->MaxPosture = 180.f;
        if (CommittedMovement) CommittedMovement->MaxWalkSpeed = 540.f;
        break;
    }

    Combat->ResetCombat();

    if (PrototypeBody && PrototypeBody->GetMaterial(0))
    {
        UMaterialInstanceDynamic* Tint = UMaterialInstanceDynamic::Create(PrototypeBody->GetMaterial(0), this);
        Tint->SetVectorParameterValue(TEXT("Color"), ArchetypeColor);
        Tint->SetVectorParameterValue(TEXT("BaseColor"), ArchetypeColor);
        TArray<UStaticMeshComponent*> Parts;
        GetComponents(Parts);
        for (UStaticMeshComponent* Part : Parts) Part->SetMaterial(0, Tint);
    }

    FlankAngleOffset = FMath::RandRange(-60.f, 60.f);
}

void ASanctuaryEnemyCharacter::Tick(float DeltaSeconds)
{
    ACharacter::Tick(DeltaSeconds);

    const bool bLegImpaired = Combat->ImpairedLimbs.Contains(ECommittedLimb::LeftLeg) ||
        Combat->ImpairedLimbs.Contains(ECommittedLimb::RightLeg);

    if (CommittedMovement)
    {
        if (CommittedMovement->bWantsToSprint)
        {
            CommittedMovement->MaxWalkSpeed = bLegImpaired ? CommittedMovement->ImpairedSprintSpeed : CommittedMovement->SprintSpeed;
        }
        else
        {
            CommittedMovement->MaxWalkSpeed = bLegImpaired ? CommittedMovement->ImpairedWalkSpeed : CommittedMovement->NormalWalkSpeed;
        }
    }

    if (Combat->State == ECommittedCombatState::Defeated) return;

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!Player || Player == this) return;

    const FVector Delta = Player->GetActorLocation() - GetActorLocation();
    const float Distance = Delta.Size2D();

    if (Combat->State == ECommittedCombatState::Ready && Distance > 1.f)
    {
        SetActorRotation(FRotator(0.f, Delta.Rotation().Yaw, 0.f));
    }

    DecisionCooldown -= DeltaSeconds;

    switch (Archetype)
    {
    case ESanctuaryEnemyArchetype::Grunt:
        TickGruntAI(DeltaSeconds, Distance, Delta);
        break;
    case ESanctuaryEnemyArchetype::Bruiser:
        TickBruiserAI(DeltaSeconds, Distance, Delta);
        break;
    case ESanctuaryEnemyArchetype::Skirmisher:
        TickSkirmisherAI(DeltaSeconds, Distance, Delta);
        break;
    case ESanctuaryEnemyArchetype::Scout:
        TickScoutAI(DeltaSeconds, Distance, Delta);
        break;
    case ESanctuaryEnemyArchetype::BossRival:
        TickBossDuelAI(DeltaSeconds, Distance, Delta);
        break;
    }
}

void ASanctuaryEnemyCharacter::TickGruntAI(float DeltaSeconds, float Distance, const FVector& Delta)
{
    if (Combat->State != ECommittedCombatState::Ready) return;

    // Approaches in front arc when it has an attack token; retreats to perimeter otherwise
    if (Combat->bHasAttackToken)
    {
        if (Distance > 160.f)
        {
            AddMovementInput(Delta.GetSafeNormal2D(), 0.7f);
        }
        else if (DecisionCooldown <= 0.f)
        {
            DecisionCooldown = 1.2f;
            Combat->StartAttack(LightAttack);
        }
    }
    else
    {
        // Maintain 3m perimeter
        if (Distance < 300.f)
        {
            AddMovementInput(-Delta.GetSafeNormal2D(), 0.5f);
        }
    }
}

void ASanctuaryEnemyCharacter::TickBruiserAI(float DeltaSeconds, float Distance, const FVector& Delta)
{
    if (Combat->State != ECommittedCombatState::Ready) return;

    if (Distance > 180.f)
    {
        AddMovementInput(Delta.GetSafeNormal2D(), 0.5f);
    }
    else if (DecisionCooldown <= 0.f)
    {
        DecisionCooldown = 1.8f;
        // Heavy, high-posture swing
        Combat->StartAttack(CommittedStrike);
    }
}

void ASanctuaryEnemyCharacter::TickSkirmisherAI(float DeltaSeconds, float Distance, const FVector& Delta)
{
    if (Combat->State != ECommittedCombatState::Ready) return;

    // Circles to player's flank
    const FVector FlankDir = FRotator(0.f, FlankAngleOffset, 0.f).RotateVector(Delta.GetSafeNormal2D());
    AddMovementInput(FlankDir, 0.9f);

    if (Distance < 220.f && DecisionCooldown <= 0.f && Combat->bHasAttackToken)
    {
        DecisionCooldown = 1.5f;
        // Lunge sweep strike
        Combat->StartAttack(SlideAttack);
    }
}

void ASanctuaryEnemyCharacter::TickScoutAI(float DeltaSeconds, float Distance, const FVector& Delta)
{
    if (Combat->State != ECommittedCombatState::Ready) return;

    // Maintains 9m-14m distance
    if (Distance < 900.f)
    {
        AddMovementInput(-Delta.GetSafeNormal2D(), 0.8f);
    }
    else if (Distance > 1400.f)
    {
        AddMovementInput(Delta.GetSafeNormal2D(), 0.6f);
    }
    else if (DecisionCooldown <= 0.f && Combat->bHasAttackToken)
    {
        DecisionCooldown = 2.4f;
        // Telegraphed ranged strike placeholder
        Combat->StartAttack(LightAttack);
    }
}

void ASanctuaryEnemyCharacter::TickBossDuelAI(float DeltaSeconds, float Distance, const FVector& Delta)
{
    if (Combat->State != ECommittedCombatState::Ready) return;

    // Boss Phase 2 Check
    if (Combat->Health <= (Combat->MaxHealth * 0.5f))
    {
        bBossClashPhaseActive = true;
    }

    if (Distance > 200.f)
    {
        if (CommittedMovement) CommittedMovement->SetWantsToSprint(true);
        AddMovementInput(Delta.GetSafeNormal2D(), 1.0f);
    }
    else
    {
        if (CommittedMovement) CommittedMovement->SetWantsToSprint(false);

        if (DecisionCooldown <= 0.f)
        {
            DecisionCooldown = 0.9f;

            // Sekiro rhythmic attack string execution
            BossAttackComboCounter = (BossAttackComboCounter + 1) % 4;

            if (BossAttackComboCounter == 0)
            {
                Combat->StartAttack(CommittedStrike);
            }
            else if (BossAttackComboCounter == 1 || BossAttackComboCounter == 2)
            {
                Combat->StartAttack(LightAttack);
            }
            else
            {
                Combat->StartAttack(DashAttack);
            }
        }
    }
}
