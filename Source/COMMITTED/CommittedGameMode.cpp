#include "CommittedGameMode.h"
#include "CommittedPrototypeHUD.h"
#include "Combat/CommittedFighterCharacter.h"
#include "Combat/CommittedFighterRegistry.h"
#include "Combat/SanctuaryEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ACommittedGameMode::ACommittedGameMode()
{
    DefaultPawnClass = ACommittedFighterCharacter::StaticClass();
    HUDClass = ACommittedPrototypeHUD::StaticClass();
    CrowdDirector = CreateDefaultSubobject<UCommittedCrowdDirectorComponent>(TEXT("CrowdDirector"));
}

void ACommittedGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (!GetWorld()) return;
    const FString CurrentMap = GetWorld()->GetMapName();

    // Initialize default fighter profile for player
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ACommittedFighterCharacter* PlayerChar = Cast<ACommittedFighterCharacter>(PC->GetPawn()))
        {
            FCommittedFighterDefinition DefaultFighter;
            if (UCommittedFighterRegistry::GetFighterDefinition(TEXT("MuayThai"), DefaultFighter))
            {
                PlayerChar->ApplyFighterDefinition(DefaultFighter);
            }
        }
    }

    if (CurrentMap.Contains(TEXT("Sanctuary")))
    {
        SpawnSanctuaryEncounter();
    }
    else if (CurrentMap.Contains(TEXT("PrototypeDuel")))
    {
        const FTransform RivalTransform(FRotator(0.f, 180.f, 0.f), FVector(280.f, 0.f, 100.f));
        ACommittedFighterCharacter* Rival = GetWorld()->SpawnActorDeferred<ACommittedFighterCharacter>(
            ACommittedFighterCharacter::StaticClass(), RivalTransform);
        if (Rival)
        {
            Rival->bPrototypeRivalAI = true;
            Rival->FinishSpawning(RivalTransform);
            Rival->SpawnDefaultController();
            if (CrowdDirector && Rival->Combat)
            {
                CrowdDirector->RegisterEnemy(Rival->Combat);
            }
        }
    }
}

void ACommittedGameMode::OnFighterConfirmed(const FCommittedFighterDefinition& FighterDef)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ACommittedFighterCharacter* PlayerChar = Cast<ACommittedFighterCharacter>(PC->GetPawn()))
        {
            PlayerChar->ApplyFighterDefinition(FighterDef);
        }
    }
}

void ACommittedGameMode::SpawnSanctuaryEncounter()
{
    auto SpawnArchetype = [this](ESanctuaryEnemyArchetype Archetype, FVector Location, FRotator Rotation)
    {
        const FTransform Xform(Rotation, Location);
        ASanctuaryEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<ASanctuaryEnemyCharacter>(
            ASanctuaryEnemyCharacter::StaticClass(), Xform);
        if (Enemy)
        {
            Enemy->Archetype = Archetype;
            Enemy->FinishSpawning(Xform);
            Enemy->SpawnDefaultController();
            if (CrowdDirector && Enemy->Combat)
            {
                CrowdDirector->RegisterEnemy(Enemy->Combat);
            }
        }
        return Enemy;
    };

    // Tier 2: Canyon Bridge Patrol (Grunt & Skirmisher)
    SpawnArchetype(ESanctuaryEnemyArchetype::Grunt, FVector(0.f, 1700.f, 960.f), FRotator(0.f, -90.f, 0.f));
    SpawnArchetype(ESanctuaryEnemyArchetype::Skirmisher, FVector(200.f, 1900.f, 960.f), FRotator(0.f, -120.f, 0.f));

    // Tier 3: Shaper Shelf Perimeter (Bruiser & Scout)
    SpawnArchetype(ESanctuaryEnemyArchetype::Bruiser, FVector(-250.f, 2750.f, 560.f), FRotator(0.f, -90.f, 0.f));
    SpawnArchetype(ESanctuaryEnemyArchetype::Scout, FVector(500.f, 2900.f, 560.f), FRotator(0.f, -100.f, 0.f));

    // Tier 4: Sacred Dueling Ring Boss & Elite Retinue
    SpawnArchetype(ESanctuaryEnemyArchetype::BossRival, FVector(0.f, 5200.f, 110.f), FRotator(0.f, -90.f, 0.f));
    SpawnArchetype(ESanctuaryEnemyArchetype::Grunt, FVector(-400.f, 4900.f, 110.f), FRotator(0.f, -70.f, 0.f));
    SpawnArchetype(ESanctuaryEnemyArchetype::Skirmisher, FVector(400.f, 4900.f, 110.f), FRotator(0.f, -110.f, 0.f));
}
