#include "CommittedFighterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

ACommittedFighterCharacter::ACommittedFighterCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    Combat = CreateDefaultSubobject<UCommittedCombatComponent>(TEXT("Combat"));
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 380.f;
    CameraBoom->bUsePawnControlRotation = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 480.f;

    LightAttack.Name = TEXT("Quick strike");
    LightAttack.StartupSeconds = 0.18f;
    LightAttack.ActiveSeconds = 0.16f;
    LightAttack.RecoverySeconds = 0.28f;
    LightAttack.Damage = 15.f;

    LimbStrike.Name = TEXT("Limb strike");
    LimbStrike.StartupSeconds = 0.36f;
    LimbStrike.ActiveSeconds = 0.14f;
    LimbStrike.RecoverySeconds = 0.48f;
    LimbStrike.Damage = 18.f;
    LimbStrike.TargetLimb = ECommittedLimb::LeftLeg;

    CommittedStrike.Name = TEXT("Committed strike");
    CommittedStrike.StartupSeconds = 0.85f;
    CommittedStrike.ActiveSeconds = 0.12f;
    CommittedStrike.RecoverySeconds = 1.1f;
    CommittedStrike.Range = 150.f;
    CommittedStrike.HalfAngleDegrees = 28.f;
    CommittedStrike.bLethal = true;
}

void ACommittedFighterCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (bPrototypeRivalAI)
    {
        if (!Controller) SpawnDefaultController();
        CameraBoom->Deactivate();
        FollowCamera->Deactivate();
    }
}

void ACommittedFighterCharacter::MoveForward(float Value)
{
    if (Combat->State != ECommittedCombatState::Ready || FMath::IsNearlyZero(Value)) return;
    const FRotator Yaw(0.f, Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Value);
}

void ACommittedFighterCharacter::MoveRight(float Value)
{
    if (Combat->State != ECommittedCombatState::Ready || FMath::IsNearlyZero(Value)) return;
    const FRotator Yaw(0.f, Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Value);
}

void ACommittedFighterCharacter::Turn(float Value) { AddControllerYawInput(Value); }
void ACommittedFighterCharacter::LookUp(float Value) { AddControllerPitchInput(Value); }
void ACommittedFighterCharacter::DoLightAttack() { Combat->StartAttack(LightAttack); }
void ACommittedFighterCharacter::DoLimbStrike() { Combat->StartAttack(LimbStrike); }
void ACommittedFighterCharacter::DoCommittedStrike() { Combat->StartAttack(CommittedStrike); }
void ACommittedFighterCharacter::DoReversal() { Combat->StartReversal(); }

void ACommittedFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACommittedFighterCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACommittedFighterCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACommittedFighterCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACommittedFighterCharacter::LookUp);
    PlayerInputComponent->BindAction(TEXT("LightAttack"), IE_Pressed, this, &ACommittedFighterCharacter::DoLightAttack);
    PlayerInputComponent->BindAction(TEXT("LimbStrike"), IE_Pressed, this, &ACommittedFighterCharacter::DoLimbStrike);
    PlayerInputComponent->BindAction(TEXT("CommittedStrike"), IE_Pressed, this, &ACommittedFighterCharacter::DoCommittedStrike);
    PlayerInputComponent->BindAction(TEXT("Reversal"), IE_Pressed, this, &ACommittedFighterCharacter::DoReversal);
}

void ACommittedFighterCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    const bool bLegImpaired = Combat->ImpairedLimbs.Contains(ECommittedLimb::LeftLeg) ||
        Combat->ImpairedLimbs.Contains(ECommittedLimb::RightLeg);
    GetCharacterMovement()->MaxWalkSpeed = bLegImpaired ? 300.f : 480.f;
    if (!bPrototypeRivalAI || Combat->State == ECommittedCombatState::Defeated) return;

    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!Player || Player == this) return;
    const FVector Delta = Player->GetActorLocation() - GetActorLocation();
    const float Distance = Delta.Size2D();
    if (Combat->State == ECommittedCombatState::Ready && Distance > 1.f)
    {
        SetActorRotation(FRotator(0.f, Delta.Rotation().Yaw, 0.f));
    }

    if (Combat->State != ECommittedCombatState::Ready) return;
    RivalDecisionTime -= DeltaSeconds;
    if (Distance > 130.f && Distance < 900.f)
    {
        AddMovementInput(Delta.GetSafeNormal2D(), 0.6f);
    }
    if (RivalDecisionTime > 0.f || Distance > 170.f) return;

    // A predictable training rival: its attacks are intentionally readable.
    RivalDecisionTime = 1.25f;
    if (UCommittedCombatComponent* PlayerCombat = Player->FindComponentByClass<UCommittedCombatComponent>())
    {
        if (PlayerCombat->State == ECommittedCombatState::Startup)
        {
            Combat->StartReversal();
            return;
        }
    }
    Combat->StartAttack(FMath::RandBool() ? LightAttack : LimbStrike);
}
