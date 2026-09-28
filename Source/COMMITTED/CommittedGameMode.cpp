#include "CommittedGameMode.h"
#include "CommittedPrototypeHUD.h"
#include "Combat/CommittedFighterCharacter.h"
#include "Engine/World.h"

ACommittedGameMode::ACommittedGameMode()
{
    DefaultPawnClass = ACommittedFighterCharacter::StaticClass();
    HUDClass = ACommittedPrototypeHUD::StaticClass();
}

void ACommittedGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (!GetWorld() || !GetWorld()->GetMapName().Contains(TEXT("PrototypeDuel"))) return;

    const FTransform RivalTransform(FRotator(0.f, 180.f, 0.f), FVector(280.f, 0.f, 100.f));
    ACommittedFighterCharacter* Rival = GetWorld()->SpawnActorDeferred<ACommittedFighterCharacter>(
        ACommittedFighterCharacter::StaticClass(), RivalTransform);
    if (Rival)
    {
        Rival->bPrototypeRivalAI = true;
        Rival->FinishSpawning(RivalTransform);
        Rival->SpawnDefaultController();
    }
}
