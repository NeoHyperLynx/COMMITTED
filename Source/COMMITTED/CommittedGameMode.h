#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Combat/CommittedCrowdDirectorComponent.h"
#include "CommittedGameMode.generated.h"

UCLASS()
class COMMITTED_API ACommittedGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACommittedGameMode();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCommittedCrowdDirectorComponent> CrowdDirector;

protected:
    virtual void BeginPlay() override;

private:
    void SpawnSanctuaryEncounter();
};
