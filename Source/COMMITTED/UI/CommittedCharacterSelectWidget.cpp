#include "CommittedCharacterSelectWidget.h"
#include "Combat/CommittedFighterRegistry.h"

void UCommittedCharacterSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AvailableFighters = UCommittedFighterRegistry::GetVerified15Fighters();
    SelectedIndex = 0;

    if (AvailableFighters.Num() > 0)
    {
        OnSelectionChanged(AvailableFighters[0]);
    }
}

void UCommittedCharacterSelectWidget::SelectNextFighter()
{
    if (AvailableFighters.Num() == 0) return;

    SelectedIndex = (SelectedIndex + 1) % AvailableFighters.Num();
    OnSelectionChanged(AvailableFighters[SelectedIndex]);
}

void UCommittedCharacterSelectWidget::SelectPreviousFighter()
{
    if (AvailableFighters.Num() == 0) return;

    SelectedIndex = (SelectedIndex - 1 + AvailableFighters.Num()) % AvailableFighters.Num();
    OnSelectionChanged(AvailableFighters[SelectedIndex]);
}

void UCommittedCharacterSelectWidget::ConfirmSelection()
{
    if (AvailableFighters.IsValidIndex(SelectedIndex))
    {
        OnFighterConfirmed.Broadcast(AvailableFighters[SelectedIndex]);
    }
}

FCommittedFighterDefinition UCommittedCharacterSelectWidget::GetCurrentSelectedFighter() const
{
    if (AvailableFighters.IsValidIndex(SelectedIndex))
    {
        return AvailableFighters[SelectedIndex];
    }
    return FCommittedFighterDefinition();
}
