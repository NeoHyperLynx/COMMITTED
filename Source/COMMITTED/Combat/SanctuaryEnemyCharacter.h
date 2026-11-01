#pragma once

#include "CoreMinimal.h"
#include "Combat/CommittedFighterCharacter.h"
#include "SanctuaryEnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class ESanctuaryEnemyArchetype : uint8
{
    Grunt,        // Striker - approaches in front arc with 2-hit quick strikes
    Bruiser,      // Grappler & Heavy - armored swings, immune to light flinch
    Skirmisher,   // Agile Flanker - circles player blind spots, lunge sweep kicks
    Scout,        // Ranged Disruptor - elevated perimeter, telegraphed projectiles
    BossRival     // Kensei / Shadow Vanguard - Sekiro multi-hit clash rhythm, wall rebounds
};

UCLASS(Blueprintable)
class COMMITTED_API ASanctuaryEnemyCharacter : public ACommittedFighterCharacter
{
    GENERATED_BODY()

public:
    ASanctuaryEnemyCharacter(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanctuary|Archetype")
    ESanctuaryEnemyArchetype Archetype = ESanctuaryEnemyArchetype::Grunt;

    // Boss duel mode multi-hit string counters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanctuary|Boss")
    int32 BossAttackComboCounter = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sanctuary|Boss")
    bool bBossClashPhaseActive = false;

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

private:
    float FlankAngleOffset = 0.f;
    float DecisionCooldown = 0.f;

    void TickGruntAI(float DeltaSeconds, float DistanceToPlayer, const FVector& DeltaToPlayer);
    void TickBruiserAI(float DeltaSeconds, float DistanceToPlayer, const FVector& DeltaToPlayer);
    void TickSkirmisherAI(float DeltaSeconds, float DistanceToPlayer, const FVector& DeltaToPlayer);
    void TickScoutAI(float DeltaSeconds, float DistanceToPlayer, const FVector& DeltaToPlayer);
    void TickBossDuelAI(float DeltaSeconds, float DistanceToPlayer, const FVector& DeltaToPlayer);
};
