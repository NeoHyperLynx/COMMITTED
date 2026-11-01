#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommittedCrowdDirectorComponent.generated.h"

class UCommittedCombatComponent;

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class COMMITTED_API UCommittedCrowdDirectorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCommittedCrowdDirectorComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CrowdDirector")
    int32 MaxConcurrentAttackTokens = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CrowdDirector")
    float TokenEvaluationInterval = 0.35f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CrowdDirector")
    float HesitationDurationOnTakedown = 0.65f;

    UFUNCTION(BlueprintCallable, Category="CrowdDirector")
    void RegisterEnemy(UCommittedCombatComponent* CombatComp);

    UFUNCTION(BlueprintCallable, Category="CrowdDirector")
    void UnregisterEnemy(UCommittedCombatComponent* CombatComp);

    UFUNCTION(BlueprintCallable, Category="CrowdDirector")
    void TriggerGlobalHesitation();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

private:
    TArray<TWeakObjectPtr<UCommittedCombatComponent>> RegisteredEnemies;
    float EvaluationTimer = 0.f;

    void EvaluateAndDistributeTokens();
};
