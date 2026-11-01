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
    const FLinearColor Orange(1.f, 0.55f, 0.1f);
    const FLinearColor Green(0.25f, 1.f, 0.4f);

    DrawText(TEXT("COMMITTED  /  SANCTUARY BASTION DUEL"), Ink, 28.f, 18.f, nullptr, 1.25f);
    DrawText(TEXT("WASD Move  |  Shift Sprint  |  Ctrl/C Slide  |  Alt Flash Dash  |  Space Jump/Wall Kick"), Ink, 28.f, 46.f);
    DrawText(TEXT("LMB Strike  |  RMB Limb  |  Q Lethal  |  E Deflect (Sekiro Rhythm)  |  G Grab/Throw (Urban Reign)"), Ink, 28.f, 68.f);
    DrawText(TEXT("F Contextual Environmental Takedown (Sleeping Dogs / Path of Neo: 2m Wall / Pillar / Ledge)"), Orange, 28.f, 88.f);

    if (const APlayerController* PC = GetOwningPlayerController())
    {
        if (const ACommittedFighterCharacter* Fighter = Cast<ACommittedFighterCharacter>(PC->GetPawn()))
        {
            // Fighter Profile Card & Tradition
            const FString TraditionStr = Fighter->FighterProfile.TraditionName.IsEmpty() ? TEXT("Muay Thai") : Fighter->FighterProfile.TraditionName.ToString();
            const FString FighterNameStr = Fighter->FighterProfile.FighterName.IsEmpty() ? TEXT("Pranom") : Fighter->FighterProfile.FighterName.ToString();
            const FString IdentityStr = StaticEnum<EFighterExchangeIdentity>()->GetNameStringByValue(static_cast<int64>(Fighter->FighterProfile.ExchangeIdentity));

            FString PerkStr = TEXT("");
            switch (Fighter->FighterProfile.ExchangeIdentity)
            {
            case EFighterExchangeIdentity::Grappler:
                PerkStr = TEXT("[PERK: GRAB -> THROW -> WALL SPLAT]");
                break;
            case EFighterExchangeIdentity::StickWeapon:
                PerkStr = TEXT("[PERK: REACH + DISARM-ON-DEFLECT]");
                break;
            case EFighterExchangeIdentity::BareHandStriker:
                PerkStr = TEXT("[PERK: FAST DEFLECT -> COUNTER STRINGS]");
                break;
            case EFighterExchangeIdentity::KickingFlow:
                PerkStr = TEXT("[PERK: DASH-CANCELABLE RECOVERY]");
                break;
            }

            DrawText(FString::Printf(TEXT("FIGHTER: %s  |  TRADITION: %s  |  %s  %s"),
                *FighterNameStr, *TraditionStr, *IdentityStr, *PerkStr),
                Gold, 28.f, 114.f, nullptr, 1.15f);

            // Health & Posture & Flow Meters
            DrawText(FString::Printf(TEXT("HEALTH: %.0f / %.0f"), Fighter->Combat->Health, Fighter->Combat->MaxHealth),
                Cyan, 28.f, 138.f, nullptr, 1.15f);

            const FLinearColor PostureColor = Fighter->Combat->Posture > 75.f ? Red : Orange;
            DrawText(FString::Printf(TEXT("POSTURE: %.0f / %.0f %s"), Fighter->Combat->Posture, Fighter->Combat->MaxPosture,
                Fighter->Combat->State == ECommittedCombatState::PostureBroken ? TEXT("[BROKEN!]") : TEXT("")),
                PostureColor, 28.f, 160.f, nullptr, 1.15f);

            DrawText(FString::Printf(TEXT("FLOW: %.0f / %.0f"), Fighter->Combat->Flow, Fighter->Combat->MaxFlow),
                Gold, 28.f, 182.f, nullptr, 1.15f);

            // Environmental Takedown & Grab Chain Prompts
            if (Fighter->Combat->NearbyEnvironmentalSurface != EEnvironmentalSurfaceType::None)
            {
                FString SurfName = TEXT("WALL");
                if (Fighter->Combat->NearbyEnvironmentalSurface == EEnvironmentalSurfaceType::Pillar) SurfName = TEXT("PILLAR");
                else if (Fighter->Combat->NearbyEnvironmentalSurface == EEnvironmentalSurfaceType::Ledge) SurfName = TEXT("LEDGE / CHASM");

                if (Fighter->Combat->GrabbedOpponent.IsValid())
                {
                    DrawText(FString::Printf(TEXT("[F] ENVIRONMENTAL GRAB-SLAM (%s)"), *SurfName),
                        Green, 28.f, 210.f, nullptr, 1.25f);
                }
                else
                {
                    DrawText(FString::Printf(TEXT("[F] ENVIRONMENTAL TAKEDOWN READY (%s)"), *SurfName),
                        Green, 28.f, 210.f, nullptr, 1.25f);
                }
            }
            else if (Fighter->Combat->GrabbedOpponent.IsValid())
            {
                DrawText(TEXT("[G + DIR] THROW ENEMY INTO HAZARD"), Gold, 28.f, 210.f, nullptr, 1.25f);
            }

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
                    ModeColor, 28.f, 240.f, nullptr, 1.1f);
            }
        }
    }
}
