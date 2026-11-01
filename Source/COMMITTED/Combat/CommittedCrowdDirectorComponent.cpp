#include "CommittedCrowdDirectorComponent.h"
#include "CommittedCombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCommittedCrowdDirectorComponent::UCommittedCrowdDirectorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCommittedCrowdDirectorComponent::BeginPlay()
{
    Super::BeginPlay();
    EvaluationTimer = TokenEvaluationInterval;
}

void UCommittedCrowdDirectorComponent::RegisterEnemy(UCommittedCombatComponent* CombatComp)
{
    if (CombatComp && !RegisteredEnemies.Contains(CombatComp))
    {
        RegisteredEnemies.Add(CombatComp);
    }
}

void UCommittedCrowdDirectorComponent::UnregisterEnemy(UCommittedCombatComponent* CombatComp)
{
    RegisteredEnemies.Remove(CombatComp);
}

void UCommittedCrowdDirectorComponent::TriggerGlobalHesitation(float Duration)
{
    const float ActualDuration = (Duration > 0.f) ? Duration : HesitationDurationOnTakedown;

    for (auto& EnemyPtr : RegisteredEnemies)
    {
        if (EnemyPtr.IsValid())
        {
            EnemyPtr->bHasAttackToken = false;
            // Interrupt in-progress attacks and freeze/stagger active swingers
            if (EnemyPtr->State == ECommittedCombatState::Startup || EnemyPtr->State == ECommittedCombatState::Active)
            {
                EnemyPtr->Stagger(ActualDuration);
            }
        }
    }
    EvaluationTimer = ActualDuration;
}

void UCommittedCrowdDirectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    EvaluationTimer -= DeltaTime;
    if (EvaluationTimer <= 0.f)
    {
        EvaluationTimer = TokenEvaluationInterval;
        EvaluateAndDistributeTokens();
    }
}

void UCommittedCrowdDirectorComponent::EvaluateAndDistributeTokens()
{
    RegisteredEnemies.RemoveAll([](const TWeakObjectPtr<UCommittedCombatComponent>& Ptr)
    {
        return !Ptr.IsValid() || Ptr->State == ECommittedCombatState::Defeated;
    });

    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!PlayerChar) return;

    const FVector PlayerLoc = PlayerChar->GetActorLocation();

    // Sort valid enemies by distance to player
    RegisteredEnemies.Sort([PlayerLoc](const TWeakObjectPtr<UCommittedCombatComponent>& A, const TWeakObjectPtr<UCommittedCombatComponent>& B)
    {
        if (!A.IsValid() || !B.IsValid()) return false;
        const float DistA = FVector::DistSquared(A->GetOwner()->GetActorLocation(), PlayerLoc);
        const float DistB = FVector::DistSquared(B->GetOwner()->GetActorLocation(), PlayerLoc);
        return DistA < DistB;
    });

    int32 GrantedTokens = 0;
    for (auto& EnemyPtr : RegisteredEnemies)
    {
        if (!EnemyPtr.IsValid()) continue;

        if (GrantedTokens < MaxConcurrentAttackTokens && EnemyPtr->State == ECommittedCombatState::Ready)
        {
            EnemyPtr->bHasAttackToken = true;
            GrantedTokens++;
        }
        else
        {
            EnemyPtr->bHasAttackToken = false;
        }
    }
}
