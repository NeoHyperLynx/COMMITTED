#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CommittedCombatComponent.h"
#include "CommittedFighterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(Blueprintable)
class COMMITTED_API ACommittedFighterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACommittedFighterCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCommittedCombatComponent> Combat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="COMMITTED")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    bool bPrototypeRivalAI = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    FCommittedAttack LightAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    FCommittedAttack LimbStrike;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="COMMITTED|Prototype")
    FCommittedAttack CommittedStrike;

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

private:
    float RivalDecisionTime = 0.f;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void DoLightAttack();
    void DoLimbStrike();
    void DoCommittedStrike();
    void DoReversal();
};
