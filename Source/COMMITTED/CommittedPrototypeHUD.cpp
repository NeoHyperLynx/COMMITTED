#include "CommittedPrototypeHUD.h"
#include "Combat/CommittedFighterCharacter.h"
#include "GameFramework/PlayerController.h"

void ACommittedPrototypeHUD::DrawHUD()
{
    Super::DrawHUD();
    const FLinearColor Ink(0.9f, 0.95f, 1.f);
    DrawText(TEXT("COMMITTED  /  PROTOTYPE DUEL"), Ink, 28.f, 24.f, nullptr, 1.25f);
    DrawText(TEXT("WASD move  |  Mouse look  |  Left click quick strike  |  Right click limb strike"), Ink, 28.f, 58.f);
    DrawText(TEXT("Q committed lethal strike  |  E timed reversal  |  Esc releases mouse"), Ink, 28.f, 82.f);
    DrawText(TEXT("Read the rival's startup. Reversals work only from the front and have recovery on a miss."),
        Ink, 28.f, 112.f);
    if (const APlayerController* PC = GetOwningPlayerController())
    {
        if (const ACommittedFighterCharacter* Fighter = Cast<ACommittedFighterCharacter>(PC->GetPawn()))
        {
            DrawText(FString::Printf(TEXT("HEALTH  %.0f / %.0f"), Fighter->Combat->Health,
                Fighter->Combat->MaxHealth), FLinearColor(0.2f, 0.9f, 1.f), 28.f, 150.f, nullptr, 1.2f);
        }
    }
}
