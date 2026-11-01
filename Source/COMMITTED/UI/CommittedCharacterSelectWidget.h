#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Combat/CommittedFighterDefinition.h"
#include "CommittedCharacterSelectWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFighterConfirmedDelegate, const FCommittedFighterDefinition&, SelectedFighter);

UCLASS(Abstract, Blueprintable)
class COMMITTED_API UCommittedCharacterSelectWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UPROPERTY(BlueprintAssignable, Category="COMMITTED|Select")
    FOnFighterConfirmedDelegate OnFighterConfirmed;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Select")
    TArray<FCommittedFighterDefinition> AvailableFighters;

    UPROPERTY(BlueprintReadOnly, Category="COMMITTED|Select")
    int32 SelectedIndex = 0;

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Select")
    void SelectNextFighter();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Select")
    void SelectPreviousFighter();

    UFUNCTION(BlueprintCallable, Category="COMMITTED|Select")
    void ConfirmSelection();

    UFUNCTION(BlueprintPure, Category="COMMITTED|Select")
    FCommittedFighterDefinition GetCurrentSelectedFighter() const;

protected:
    UFUNCTION(BlueprintImplementableEvent, Category="COMMITTED|Select")
    void OnSelectionChanged(const FCommittedFighterDefinition& NewFighter);
};
