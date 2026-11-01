#include "CommittedPrototypeHUD.h"
#include "Combat/CommittedFighterCharacter.h"
#include "Combat/CommittedMovementComponent.h"
#include "GameFramework/PlayerController.h"

void ACommittedPrototypeHUD::DrawHUD()
{
    Super::DrawHUD();
    const FLinearColor Ink(0.9f, 0.95f, 1.f);
    const FLinearColor Cyan(0.2f, 0.9f, 1.f);
    const FLinearColor Gold(1.f, 0.82f, 0.2f);
    const FLinearColor Red(1.f, 0.35f, 0.35f);

    DrawText(TEXT("COMMITTED  /  PHANTOM AGILITY DUEL"), Ink, 28.f, 20.f, nullptr, 1.25f);
    DrawText(TEXT("WASD move  |  Shift Sprint  |  Ctrl/C Slide  |  Alt/F Flash Dash  |  Space Jump/Wall Kick"), Ink, 28.f, 50.f);
    DrawText(TEXT("LMB Strike / Dash / Slide Attack  |  RMB Limb Strike  |  Q Committed Strike  |  E Reversal"), Ink, 28.f, 72.f);
    DrawText(TEXT("Run along vertical walls to Wall Run; jump to Wall Kick. Dash cancels recovery into counter positions."),
        Ink, 28.f, 96.f);

    if (const APlayerController* PC = GetOwningPlayerController())
    {
        if (const ACommittedFighterCharacter* Fighter = Cast<ACommittedFighterCharacter>(PC->GetPawn()))
        {
            DrawText(FString::Printf(TEXT("HEALTH  %.0f / %.0f"), Fighter->Combat->Health,
                Fighter->Combat->MaxHealth), Cyan, 28.f, 130.f, nullptr, 1.15f);

            if (const UCommittedMovementComponent* Mov = Fighter->CommittedMovement)
            {
                FString ModeText = TEXT("WALKING");
                FLinearColor ModeColor = Cyan;

                if (Mov->IsCustomMovementMode(ECommittedCustomMovementMode::Dash))
                {
                    ModeText = TEXT("FLASH DASH");
                    ModeColor = Gold;
                }
                else if (Mov->IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
                {
                    ModeText = TEXT("SLIDING");
                    ModeColor = Gold;
                }
                else if (Mov->IsCustomMovementMode(ECommittedCustomMovementMode::WallRun))
                {
                    ModeText = Mov->CurrentWallRunSide == ECommittedWallRunSide::Left
                        ? TEXT("WALL RUN (LEFT)") : TEXT("WALL RUN (RIGHT)");
                    ModeColor = Gold;
                }
                else if (Mov->bWantsToSprint)
                {
                    ModeText = TEXT("SPRINTING");
                    ModeColor = Gold;
                }
                else if (Mov->IsFalling())
                {
                    ModeText = TEXT("AIRBORNE");
                }

                DrawText(FString::Printf(TEXT("SPEED: %.0f  |  MODE: %s  |  PHANTOM CHARGES: %.1f / %d"),
                    Mov->Velocity.Size2D(), *ModeText, Mov->AvailableDashCharges, Mov->MaxDashCharges),
                    ModeColor, 28.f, 155.f, nullptr, 1.1f);
            }
        }
    }
}
