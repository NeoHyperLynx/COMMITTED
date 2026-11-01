#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Combat/CommittedCrowdDirectorComponent.h"
#include "UI/CommittedCharacterSelectWidget.h"
#include "CommittedGameMode.generated.h"

UCLASS()
class COMMITTED_API ACommittedGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACommittedGameMode();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCommittedCrowdDirectorComponent> CrowdDirector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|UI")
    TSubclassOf<UCommittedCharacterSelectWidget> CharacterSelectWidgetClass;

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Select")
    void OnFighterConfirmed(const FCommittedFighterDefinition& FighterDef);

protected:
    virtual void BeginPlay() override;

private:
    void SpawnSanctuaryEncounter();
};
