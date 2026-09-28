#include "CommittedFighterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
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
    RefreshPrototypeLabel(Combat->State, NAME_None);
    if (bPrototypeRivalAI)
    {
        if (!Controller) SpawnDefaultController();
        CameraBoom->Deactivate();
        FollowCamera->Deactivate();
    }
}

void ACommittedFighterCharacter::RefreshPrototypeLabel(ECommittedCombatState NewState, FName MoveName)
{
    const FString StateName = StaticEnum<ECommittedCombatState>()->GetNameStringByValue(
        static_cast<int64>(NewState));
    PrototypeLabel->SetText(FText::FromString(FString::Printf(TEXT("%s  %.0f HP  |  %s"),
        bPrototypeRivalAI ? TEXT("RIVAL") : TEXT("YOU"), Combat->Health, *StateName)));
    const FColor Base = bPrototypeRivalAI ? FColor(255, 95, 65) : FColor(65, 220, 255);
    PrototypeLabel->SetTextRenderColor(NewState == ECommittedCombatState::Reversal ? FColor::Green :
        NewState == ECommittedCombatState::Startup ? FColor::Yellow :
        NewState == ECommittedCombatState::Defeated ? FColor::Red : Base);
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
