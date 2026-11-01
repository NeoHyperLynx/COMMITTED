#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CommittedCombatComponent.h"
#include "CommittedMovementComponent.h"
#include "CommittedFighterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class COMMITTED_API ACommittedFighterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACommittedFighterCharacter(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCommittedCombatComponent> Combat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCommittedMovementComponent> CommittedMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED|Prototype")
    TObjectPtr<UStaticMeshComponent> PrototypeBody;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED|Prototype")
    TObjectPtr<UTextRenderComponent> PrototypeLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    bool bPrototypeRivalAI = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    FCommittedAttack LightAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    FCommittedAttack LimbStrike;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    FCommittedAttack CommittedStrike;

    // Movement-integrated attacks
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|MovementCombat")
    FCommittedAttack DashAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|MovementCombat")
    FCommittedAttack SlideAttack;

    // Camera dynamic FOV tuning
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Camera")
    float BaseFOV = 90.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Camera")
    float SprintFOV = 102.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Camera")
    float DashFOV = 110.f;

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void RefreshPrototypeLabel(ECommittedCombatState NewState, FName MoveName);

    UFUNCTION()
    void OnCustomMovementModeChanged(ECommittedCustomMovementMode NewCustomMode);

private:
    float RivalDecisionTime = 0.f;
    float LastInputForward = 0.f;
    float LastInputRight = 0.f;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    void DoSprintStart();
    void DoSprintStop();
    void DoSlideStart();
    void DoSlideStop();
    void DoDash();
    void DoJumpAction();
    void DoStopJumpingAction();

    void DoLightAttack();
    void DoLimbStrike();
    void DoCommittedStrike();
    void DoReversal();

    void UpdateDynamicCamera(float DeltaSeconds);
};
