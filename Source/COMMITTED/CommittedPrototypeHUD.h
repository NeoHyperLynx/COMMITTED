#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CommittedPrototypeHUD.generated.h"

UCLASS()
class COMMITTED_API ACommittedPrototypeHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
};
