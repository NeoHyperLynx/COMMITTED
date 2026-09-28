#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat/DevotedCombatComponent.h"
#include "DevotedFighterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(Blueprintable)
class DEVOTED_API ADevotedFighterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADevotedFighterCharacter();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DEVOTED")
    TObjectPtr<UDevotedCombatComponent> Combat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DEVOTED")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="DEVOTED")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Prototype")
    bool bPrototypeRivalAI = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Prototype")
    FDevotedAttack LightAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Prototype")
    FDevotedAttack LimbStrike;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DEVOTED|Prototype")
    FDevotedAttack CommittedStrike;

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
