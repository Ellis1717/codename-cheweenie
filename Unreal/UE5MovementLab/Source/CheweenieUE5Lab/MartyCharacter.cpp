#include "MartyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Canvas.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "UObject/ConstructorHelpers.h"

AMartyCharacter::AMartyCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(22.f, 28.f);
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 360, 0);
    GetCharacterMovement()->MaxWalkSpeed = 100;
    GetCharacterMovement()->MaxAcceleration = 600;
    GetCharacterMovement()->BrakingDecelerationWalking = 700;
    GetCharacterMovement()->MaxStepHeight = 12;
    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RetainedMarty"));
    Visual->SetupAttachment(GetRootComponent());
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // Native asset audit: display mesh is 212 x 92 x 141 cm. Traversal uses 32%.
    Visual->SetRelativeScale3D(FVector(.32f));
    Visual->SetRelativeLocation(FVector(0, 0, -28));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Pet(TEXT("/Game/VisualLab/Pets/Marty.Marty"));
    if (Pet.Succeeded()) Visual->SetStaticMesh(Pet.Object);
    Boom = CreateDefaultSubobject<USpringArmComponent>(TEXT("FollowingCamera"));
    Boom->SetupAttachment(GetRootComponent());
    Boom->TargetArmLength = 220;
    Boom->SocketOffset = FVector(0, 0, 35);
    Boom->bUsePawnControlRotation = true;
    Boom->bEnableCameraLag = true;
    Boom->CameraLagSpeed = 8;
    auto* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(Boom);
    Camera->FieldOfView = 65;
}

void AMartyCharacter::BeginPlay()
{
    Super::BeginPlay();
    TeleportTo(FVector(-200, -200, 60), FRotator(0, 40, 0));
    Home = GetActorLocation();
    Recenter();
}

void AMartyCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (auto* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->ViewPitchMin = -55;
            PC->PlayerCameraManager->ViewPitchMax = 20;
        }
    }
    Recenter();
}

void AMartyCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
    Super::SetupPlayerInputComponent(Input);
    auto* Enhanced = CastChecked<UEnhancedInputComponent>(Input);
    Mapping = NewObject<UInputMappingContext>(this);
    auto Action = [this](EInputActionValueType Type)
    {
        UInputAction* Result = NewObject<UInputAction>(this);
        Result->ValueType = Type;
        return Result;
    };
    MoveAction = Action(EInputActionValueType::Axis2D);
    MouseAction = Action(EInputActionValueType::Axis2D);
    StickAction = Action(EInputActionValueType::Axis2D);
    SprintAction = Action(EInputActionValueType::Boolean);
    CameraAction = Action(EInputActionValueType::Boolean);
    RecenterAction = Action(EInputActionValueType::Boolean);
    HomeAction = Action(EInputActionValueType::Boolean);
    auto Axis = [this](UInputAction* Target, FKey Key, bool Y, bool Negative)
    {
        auto& Entry = Mapping->MapKey(Target, Key);
        if (Negative) Entry.Modifiers.Add(NewObject<UInputModifierNegate>(Mapping));
        if (Y)
        {
            auto* Swizzle = NewObject<UInputModifierSwizzleAxis>(Mapping);
            Swizzle->Order = EInputAxisSwizzle::YXZ;
            Entry.Modifiers.Add(Swizzle);
        }
    };
    Axis(MoveAction, EKeys::W, true, false);
    Axis(MoveAction, EKeys::S, true, true);
    Axis(MoveAction, EKeys::A, false, true);
    Axis(MoveAction, EKeys::D, false, false);
    Axis(MoveAction, EKeys::Up, true, false);
    Axis(MoveAction, EKeys::Down, true, true);
    Axis(MoveAction, EKeys::Left, false, true);
    Axis(MoveAction, EKeys::Right, false, false);
    Axis(MoveAction, EKeys::Gamepad_LeftX, false, false);
    Axis(MoveAction, EKeys::Gamepad_LeftY, true, false);
    Axis(MouseAction, EKeys::MouseX, false, false);
    Axis(MouseAction, EKeys::MouseY, true, true);
    Axis(StickAction, EKeys::Gamepad_RightX, false, false);
    Axis(StickAction, EKeys::Gamepad_RightY, true, true);
    Mapping->MapKey(SprintAction, EKeys::LeftShift);
    Mapping->MapKey(SprintAction, EKeys::Gamepad_FaceButton_Bottom);
    Mapping->MapKey(CameraAction, EKeys::V);
    Mapping->MapKey(CameraAction, EKeys::Gamepad_RightThumbstick);
    Mapping->MapKey(RecenterAction, EKeys::R);
    Mapping->MapKey(RecenterAction, EKeys::Gamepad_RightShoulder);
    Mapping->MapKey(HomeAction, EKeys::H);
    Mapping->MapKey(HomeAction, EKeys::Gamepad_Special_Right);
    Enhanced->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMartyCharacter::Move);
    Enhanced->BindAction(MouseAction, ETriggerEvent::Triggered, this, &AMartyCharacter::Look);
    Enhanced->BindAction(StickAction, ETriggerEvent::Triggered, this, &AMartyCharacter::LookStick);
    Enhanced->BindAction(SprintAction, ETriggerEvent::Started, this, &AMartyCharacter::SetSprint, true);
    Enhanced->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMartyCharacter::SetSprint, false);
    Enhanced->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AMartyCharacter::SetSprint, false);
    Enhanced->BindAction(CameraAction, ETriggerEvent::Started, this, &AMartyCharacter::CycleCamera);
    Enhanced->BindAction(RecenterAction, ETriggerEvent::Started, this, &AMartyCharacter::Recenter);
    Enhanced->BindAction(HomeAction, ETriggerEvent::Started, this, &AMartyCharacter::ReturnHome);
    if (auto* PC = Cast<APlayerController>(GetController()))
        if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Subsystem->AddMappingContext(Mapping, 0);
}

void AMartyCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller) return;
    FVector2D Axis = Value.Get<FVector2D>();
    Axis = Axis.GetClampedToMaxSize(1);
    const FRotator Heading(0, Controller->GetControlRotation().Yaw, 0);
    AddMovementInput(Heading.Vector(), Axis.Y);
    AddMovementInput(FRotationMatrix(Heading).GetUnitAxis(EAxis::Y), Axis.X);
}
void AMartyCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}
void AMartyCharacter::LookStick(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>() * (80.f * GetWorld()->GetDeltaSeconds());
    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}
void AMartyCharacter::SetSprint(bool Active) { GetCharacterMovement()->MaxWalkSpeed = Active ? 230 : 100; }
void AMartyCharacter::CycleCamera()
{
    CameraPreset = (CameraPreset + 1) % 3;
    const float Distances[] = {220, 330, 140};
    const float Heights[] = {35, 90, 0};
    Boom->TargetArmLength = Distances[CameraPreset];
    Boom->SocketOffset.Z = Heights[CameraPreset];
}
void AMartyCharacter::Recenter()
{
    if (Controller) Controller->SetControlRotation(FRotator(-12, GetActorRotation().Yaw, 0));
}
void AMartyCharacter::ReturnHome()
{
    GetCharacterMovement()->StopMovementImmediately();
    TeleportTo(Home, FRotator::ZeroRotator);
    Recenter();
}

AMartyPlayMode::AMartyPlayMode()
{
    DefaultPawnClass = AMartyCharacter::StaticClass();
    HUDClass = AMartyHUD::StaticClass();
}
void AMartyPlayMode::BeginPlay()
{
    Super::BeginPlay();
    // The comparison map remains unmodified; replace only its duplicate Marty at runtime.
    for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
    {
        const auto* Mesh = It->GetStaticMeshComponent()->GetStaticMesh();
        if (!Mesh) continue;
        const FString Path = Mesh->GetPathName();
        if (Path == TEXT("/Game/VisualLab/Pets/Marty.Marty")) It->Destroy();
        else if (Path == TEXT("/Game/VisualLab/Pets/Lakitu.Lakitu") || Path == TEXT("/Game/VisualLab/Pets/Rio.Rio"))
            It->SetActorScale3D(FVector(.32f));
    }
    // Invisible boundaries protect the open photography-room edges.
    for (int32 Index = 0; Index < 4; ++Index)
    {
        auto* Wall = GetWorld()->SpawnActor<AActor>();
        auto* Box = NewObject<UBoxComponent>(Wall);
        Wall->SetRootComponent(Box);
        Box->SetBoxExtent(Index < 2 ? FVector(10, 510, 180) : FVector(510, 10, 180));
        Box->SetCollisionProfileName(TEXT("BlockAll"));
        Box->RegisterComponent();
        Wall->SetActorLocation(Index < 2 ? FVector(Index == 0 ? -490 : 490, 0, 150)
                                        : FVector(0, Index == 2 ? -490 : 490, 150));
    }
}

void AMartyHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas) return;
    DrawRect(FLinearColor(0.025f, 0.035f, 0.04f, .88f), 16, 16, 700, 100);
    DrawText(TEXT("Codename .... Cheweenie | Marty movement lab"), FLinearColor::White, 28, 25);
    DrawText(TEXT("Move: WASD / arrows / left stick   |   Look: mouse / right stick"), FLinearColor::White, 28, 48);
    DrawText(TEXT("Hurry: Shift / A-Cross   |   Camera: V / R3   |   Recenter: R / RB-R1"), FLinearColor::White, 28, 69);
    DrawText(TEXT("Return home: H / Start-Options   |   Static model: animation art is pending"), FLinearColor::White, 28, 90);
}
