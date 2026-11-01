#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommittedFighterDefinition.h"
#include "CommittedFighterRegistry.generated.h"

UCLASS(BlueprintType)
class COMMITTED_API UCommittedFighterRegistry : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="COMMITTED|FighterRegistry")
    static TArray<FCommittedFighterDefinition> GetVerified15Fighters();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|FighterRegistry")
    static bool GetFighterDefinition(FName RowID, FCommittedFighterDefinition& OutDef);
};
