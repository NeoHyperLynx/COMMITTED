#include "CommittedFighterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

ACommittedFighterCharacter::ACommittedFighterCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UCommittedMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    // Retrieve movement component cast as our custom component
    CommittedMovement = Cast<UCommittedMovementComponent>(GetCharacterMovement());

    Combat = CreateDefaultSubobject<UCommittedCombatComponent>(TEXT("Combat"));
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 380.f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 12.f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    FollowCamera->SetFieldOfView(BaseFOV);

    UStaticMesh* Block = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    auto AddPart = [this, Block](FName Name, FVector Position, FVector Scale)
    {
        UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
        Part->SetupAttachment(GetCapsuleComponent());
        Part->SetStaticMesh(Block);
        Part->SetRelativeLocation(Position);
        Part->SetRelativeScale3D(Scale);
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        return Part;
    };
    PrototypeBody = AddPart(TEXT("PrototypeBody"), FVector::ZeroVector, FVector(0.48f, 0.35f, 1.0f));
    AddPart(TEXT("LeftArm"), FVector(0.f, -34.f, 5.f), FVector(0.14f, 0.14f, 0.72f));
    AddPart(TEXT("RightArm"), FVector(0.f, 34.f, 5.f), FVector(0.14f, 0.14f, 0.72f));
    AddPart(TEXT("LeftLeg"), FVector(0.f, -17.f, -55.f), FVector(0.18f, 0.18f, 0.72f));
    AddPart(TEXT("RightLeg"), FVector(0.f, 17.f, -55.f), FVector(0.18f, 0.18f, 0.72f));
    UStaticMeshComponent* Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrototypeHead"));
    Head->SetupAttachment(GetCapsuleComponent());
    Head->SetStaticMesh(Sphere);
    Head->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
    Head->SetRelativeScale3D(FVector(0.32f));
    Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    PrototypeLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PrototypeLabel"));
    PrototypeLabel->SetupAttachment(GetCapsuleComponent());
    PrototypeLabel->SetRelativeLocation(FVector(0.f, 0.f, 126.f));
    PrototypeLabel->SetHorizontalAlignment(EHTA_Center);
    PrototypeLabel->SetWorldSize(28.f);
    PrototypeLabel->SetText(FText::FromString(TEXT("FIGHTER")));

    bUseControllerRotationYaw = false;
    if (CommittedMovement)
    {
        CommittedMovement->bOrientRotationToMovement = true;
        CommittedMovement->RotationRate = FRotator(0.f, 850.f, 0.f);
    }

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

    // Movement-integrated attacks
    DashAttack.Name = TEXT("Phantom Flash Strike");
    DashAttack.StartupSeconds = 0.12f;
    DashAttack.ActiveSeconds = 0.20f;
    DashAttack.RecoverySeconds = 0.30f;
    DashAttack.Range = 210.f;
    DashAttack.HalfAngleDegrees = 50.f;
    DashAttack.Damage = 22.f;

    SlideAttack.Name = TEXT("Low Sweep Strike");
    SlideAttack.StartupSeconds = 0.14f;
    SlideAttack.ActiveSeconds = 0.22f;
    SlideAttack.RecoverySeconds = 0.35f;
    SlideAttack.Range = 190.f;
    SlideAttack.HalfAngleDegrees = 55.f;
    SlideAttack.Damage = 18.f;
    SlideAttack.TargetLimb = ECommittedLimb::LeftLeg;
}

void ACommittedFighterCharacter::BeginPlay()
{
    Super::BeginPlay();
    if (UMaterialInterface* BaseMaterial = PrototypeBody->GetMaterial(0))
    {
        UMaterialInstanceDynamic* Tint = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        const FLinearColor Color = bPrototypeRivalAI ? FLinearColor(0.9f, 0.16f, 0.09f)
            : FLinearColor(0.06f, 0.65f, 0.95f);
        Tint->SetVectorParameterValue(TEXT("Color"), Color);
        Tint->SetVectorParameterValue(TEXT("BaseColor"), Color);
        TArray<UStaticMeshComponent*> Parts;
        GetComponents(Parts);
        for (UStaticMeshComponent* Part : Parts) Part->SetMaterial(0, Tint);
    }
    Combat->OnCombatStateChanged.AddDynamic(this, &ACommittedFighterCharacter::RefreshPrototypeLabel);
    if (CommittedMovement)
    {
        CommittedMovement->OnCustomMovementModeChanged.AddDynamic(this, &ACommittedFighterCharacter::OnCustomMovementModeChanged);
    }

    RefreshPrototypeLabel(Combat->State, NAME_None);
    if (bPrototypeRivalAI)
    {
        if (!Controller) SpawnDefaultController();
        CameraBoom->Deactivate();
        FollowCamera->Deactivate();
    }
}

void ACommittedFighterCharacter::ApplyFighterDefinition(const FCommittedFighterDefinition& NewProfile)
{
    FighterProfile = NewProfile;

    if (Combat)
    {
        Combat->MaxPosture = NewProfile.PostureMax;
        Combat->PostureRegenRate = NewProfile.PostureRegenRate;
        Combat->FlowGainRate = NewProfile.FlowGainRate;
        Combat->MaxFlow = NewProfile.FlowMax;
        Combat->ActiveExchangeIdentity = NewProfile.ExchangeIdentity;
        Combat->ResetCombat();
    }

    if (!NewProfile.LightAttack.Name.IsNone()) LightAttack = NewProfile.LightAttack;
    if (!NewProfile.HeavyAttack.Name.IsNone()) CommittedStrike = NewProfile.HeavyAttack;

    RefreshPrototypeLabel(Combat ? Combat->State : ECommittedCombatState::Ready, NAME_None);
}

void ACommittedFighterCharacter::OnCustomMovementModeChanged(ECommittedCustomMovementMode NewCustomMode)
{
    RefreshPrototypeLabel(Combat->State, NAME_None);
}

void ACommittedFighterCharacter::RefreshPrototypeLabel(ECommittedCombatState NewState, FName MoveName)
{
    const FString StateName = StaticEnum<ECommittedCombatState>()->GetNameStringByValue(
        static_cast<int64>(NewState));

    FString MoveStatus = TEXT("");
    if (CommittedMovement)
    {
        if (CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::Dash))
        {
            MoveStatus = TEXT(" [FLASH DASH]");
        }
        else if (CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
        {
            MoveStatus = TEXT(" [SLIDING]");
        }
        else if (CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::WallRun))
        {
            MoveStatus = CommittedMovement->CurrentWallRunSide == ECommittedWallRunSide::Left
                ? TEXT(" [WALL RUN L]") : TEXT(" [WALL RUN R]");
        }
        else if (CommittedMovement->bWantsToSprint)
        {
            MoveStatus = TEXT(" [SPRINT]");
        }
    }

    PrototypeLabel->SetText(FText::FromString(FString::Printf(TEXT("%s  %.0f HP  |  %s%s"),
        bPrototypeRivalAI ? TEXT("RIVAL") : TEXT("YOU"), Combat->Health, *StateName, *MoveStatus)));

    const FColor Base = bPrototypeRivalAI ? FColor(255, 95, 65) : FColor(65, 220, 255);
    PrototypeLabel->SetTextRenderColor(NewState == ECommittedCombatState::Deflect ? FColor::Green :
        NewState == ECommittedCombatState::Startup ? FColor::Yellow :
        NewState == ECommittedCombatState::Defeated ? FColor::Red : Base);
}

void ACommittedFighterCharacter::MoveForward(float Value)
{
    LastInputForward = Value;
    if (Combat->State != ECommittedCombatState::Ready && Combat->State != ECommittedCombatState::Recovery) return;
    if (FMath::IsNearlyZero(Value)) return;
    const FRotator Yaw(0.f, Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Value);
}

void ACommittedFighterCharacter::MoveRight(float Value)
{
    LastInputRight = Value;
    if (Combat->State != ECommittedCombatState::Ready && Combat->State != ECommittedCombatState::Recovery) return;
    if (FMath::IsNearlyZero(Value)) return;
    const FRotator Yaw(0.f, Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Value);
}

void ACommittedFighterCharacter::Turn(float Value) { AddControllerYawInput(Value); }
void ACommittedFighterCharacter::LookUp(float Value) { AddControllerPitchInput(Value); }

void ACommittedFighterCharacter::DoSprintStart()
{
    if (CommittedMovement)
    {
        CommittedMovement->SetWantsToSprint(true);
    }
}

void ACommittedFighterCharacter::DoSprintStop()
{
    if (CommittedMovement)
    {
        CommittedMovement->SetWantsToSprint(false);
    }
}

void ACommittedFighterCharacter::DoSlideStart()
{
    if (!CommittedMovement) return;

    if (CommittedMovement->bWantsToSprint || CommittedMovement->Velocity.Size2D() > CommittedMovement->NormalWalkSpeed * 0.7f)
    {
        CommittedMovement->StartSlide();
    }
}

void ACommittedFighterCharacter::DoSlideStop()
{
    if (CommittedMovement && CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
    {
        CommittedMovement->StopSlide();
    }
}

void ACommittedFighterCharacter::DoDash()
{
    if (!CommittedMovement) return;

    // Kicking Flow Perk: Can ALWAYS cancel recovery frames into flash dash
    if (Combat->State == ECommittedCombatState::Recovery)
    {
        if (FighterProfile.ExchangeIdentity == EFighterExchangeIdentity::KickingFlow || Combat->Flow >= 30.f)
        {
            Combat->ResetCombat();
        }
    }

    if (Combat->State != ECommittedCombatState::Ready) return;

    // Direction based on current input axis
    FVector DashDir = FVector::ZeroVector;
    const FRotator Yaw(0.f, Controller ? Controller->GetControlRotation().Yaw : GetActorRotation().Yaw, 0.f);
    const FVector Forward = FRotationMatrix(Yaw).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y);

    DashDir = (Forward * LastInputForward) + (Right * LastInputRight);
    if (DashDir.IsNearlyZero())
    {
        DashDir = GetActorForwardVector();
    }

    CommittedMovement->StartDash(DashDir.GetSafeNormal2D());
}

void ACommittedFighterCharacter::DoJumpAction()
{
    Jump();
}

void ACommittedFighterCharacter::DoStopJumpingAction()
{
    StopJumping();
}

void ACommittedFighterCharacter::DoLightAttack()
{
    if (!CommittedMovement)
    {
        Combat->StartAttack(LightAttack);
        return;
    }

    // Branch into special attack variants if dashing or sliding
    if (CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::Dash))
    {
        Combat->StartAttack(DashAttack);
    }
    else if (CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::Slide))
    {
        Combat->StartAttack(SlideAttack);
    }
    else
    {
        Combat->StartAttack(LightAttack);
    }
}

void ACommittedFighterCharacter::DoLimbStrike() { Combat->StartAttack(LimbStrike); }
void ACommittedFighterCharacter::DoCommittedStrike() { Combat->StartAttack(CommittedStrike); }
void ACommittedFighterCharacter::DoReversal() { Combat->StartDeflect(); }
void ACommittedFighterCharacter::DoEnvironmentalTakedown() { Combat->TryEnvironmentalTakedown(); }
void ACommittedFighterCharacter::DoGrab()
{
    if (Combat->GrabbedOpponent.IsValid())
    {
        Combat->TryGrabThrow(GetActorForwardVector());
    }
    else
    {
        Combat->TryDedicatedGrab(EGrabTargetPart::Head);
    }
}

void ACommittedFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACommittedFighterCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACommittedFighterCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACommittedFighterCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACommittedFighterCharacter::LookUp);

    // Movement Actions
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ACommittedFighterCharacter::DoSprintStart);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ACommittedFighterCharacter::DoSprintStop);
    PlayerInputComponent->BindAction(TEXT("Slide"), IE_Pressed, this, &ACommittedFighterCharacter::DoSlideStart);
    PlayerInputComponent->BindAction(TEXT("Slide"), IE_Released, this, &ACommittedFighterCharacter::DoSlideStop);
    PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ACommittedFighterCharacter::DoDash);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACommittedFighterCharacter::DoJumpAction);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACommittedFighterCharacter::DoStopJumpingAction);

    // Combat Actions
    PlayerInputComponent->BindAction(TEXT("LightAttack"), IE_Pressed, this, &ACommittedFighterCharacter::DoLightAttack);
    PlayerInputComponent->BindAction(TEXT("LimbStrike"), IE_Pressed, this, &ACommittedFighterCharacter::DoLimbStrike);
    PlayerInputComponent->BindAction(TEXT("CommittedStrike"), IE_Pressed, this, &ACommittedFighterCharacter::DoCommittedStrike);
    PlayerInputComponent->BindAction(TEXT("Reversal"), IE_Pressed, this, &ACommittedFighterCharacter::DoReversal);
    PlayerInputComponent->BindAction(TEXT("EnvironmentalTakedown"), IE_Pressed, this, &ACommittedFighterCharacter::DoEnvironmentalTakedown);
    PlayerInputComponent->BindAction(TEXT("Grab"), IE_Pressed, this, &ACommittedFighterCharacter::DoGrab);
}

void ACommittedFighterCharacter::UpdateDynamicCamera(float DeltaSeconds)
{
    if (!FollowCamera || bPrototypeRivalAI) return;

    float TargetFOV = BaseFOV;
    if (CommittedMovement)
    {
        if (CommittedMovement->IsCustomMovementMode(ECommittedCustomMovementMode::Dash))
        {
            TargetFOV = DashFOV;
        }
        else if (CommittedMovement->bWantsToSprint && CommittedMovement->Velocity.Size2D() > CommittedMovement->NormalWalkSpeed)
        {
            TargetFOV = SprintFOV;
        }
    }

    const float CurrentFOV = FollowCamera->FieldOfView;
    const float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaSeconds, 8.f);
    FollowCamera->SetFieldOfView(NewFOV);
}

void ACommittedFighterCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const bool bLegImpaired = Combat->ImpairedLimbs.Contains(ECommittedLimb::LeftLeg) ||
        Combat->ImpairedLimbs.Contains(ECommittedLimb::RightLeg);

    if (CommittedMovement)
    {
        if (CommittedMovement->bWantsToSprint)
        {
            CommittedMovement->MaxWalkSpeed = bLegImpaired ? CommittedMovement->ImpairedSprintSpeed : CommittedMovement->SprintSpeed;
        }
        else
        {
            CommittedMovement->MaxWalkSpeed = bLegImpaired ? CommittedMovement->ImpairedWalkSpeed : CommittedMovement->NormalWalkSpeed;
        }
    }

    UpdateDynamicCamera(DeltaSeconds);

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

    // Rival AI behavior with mobility: close gap faster or evade
    if (Distance > 450.f && Distance < 1200.f)
    {
        if (CommittedMovement) CommittedMovement->SetWantsToSprint(true);
        AddMovementInput(Delta.GetSafeNormal2D(), 1.0f);
    }
    else if (Distance > 130.f && Distance <= 450.f)
    {
        if (CommittedMovement) CommittedMovement->SetWantsToSprint(false);
        AddMovementInput(Delta.GetSafeNormal2D(), 0.6f);
    }

    if (RivalDecisionTime > 0.f || Distance > 170.f) return;

    // Predictable training rival with agile reactions
    RivalDecisionTime = 1.25f;
    if (UCommittedCombatComponent* PlayerCombat = Player->FindComponentByClass<UCommittedCombatComponent>())
    {
        if (PlayerCombat->State == ECommittedCombatState::Startup)
        {
            // 50% chance to deflect, 50% chance to flash dash backwards
            if (FMath::RandBool())
            {
                Combat->StartDeflect();
            }
            else if (CommittedMovement)
            {
                CommittedMovement->StartDash(-Delta.GetSafeNormal2D());
            }
            return;
        }
    }
    Combat->StartAttack(FMath::RandBool() ? LightAttack : LimbStrike);
}
