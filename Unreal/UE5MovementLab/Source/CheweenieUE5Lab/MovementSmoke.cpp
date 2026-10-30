// Explicit opt-in native runtime acceptance. Synthetic device events are not physical hardware tests.
#include "MartyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "InputCoreTypes.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/EngineVersion.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "HighResScreenshot.h"
#include "UnrealClient.h"

void AMartyPlayMode::SmokeCheck(bool Passed, const FString& Name)
{
    (Passed ? SmokePassed : SmokeFailed).Add(Name);
    UE_LOG(LogTemp, Display, TEXT("CHEWEENIE_CHECK %s: %s"), Passed ? TEXT("PASS") : TEXT("FAIL"), *Name);
}

void AMartyPlayMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!FParse::Param(FCommandLine::Get(), TEXT("CheweenieSmoke"))) return;
    SmokeTime += DeltaSeconds;
    auto* PC = GetWorld()->GetFirstPlayerController();
    auto* Marty = PC ? Cast<AMartyCharacter>(PC->GetPawn()) : nullptr;
    if (!Marty)
    {
        if (SmokeTime > 15) { UE_LOG(LogTemp, Error, TEXT("CHEWEENIE_SMOKE: Marty possession missing")); FPlatformMisc::RequestExitWithStatus(false, 1); }
        return;
    }
    auto Key = [PC](FKey K, bool Down)
    {
        PC->InputKey(FInputKeyEventArgs(nullptr, INPUTDEVICEID_NONE, K, Down ? IE_Pressed : IE_Released, FPlatformTime::Cycles64()));
    };
    auto Axis = [PC, DeltaSeconds](FKey K, float Value)
    {
        PC->InputKey(FInputKeyEventArgs(nullptr, INPUTDEVICEID_NONE, K, Value, DeltaSeconds, 1, FPlatformTime::Cycles64()));
    };
    auto Next = [this]() { ++SmokeStage; SmokeTime = 0; };
    if (SmokeStage == 7) Axis(EKeys::Gamepad_LeftX, .7f);
    switch (SmokeStage)
    {
    case 0:
        if (SmokeTime < 2) break;
        SmokeCheck(Marty->GetCharacterMovement()->IsMovingOnGround(), TEXT("Spawn lands on room floor"));
        SmokePosition = Marty->GetActorLocation(); Key(EKeys::W, true); Next(); break;
    case 1:
        if (SmokeTime < 1) break;
        SmokeCheck(FVector::Dist2D(SmokePosition, Marty->GetActorLocation()) > 40, TEXT("W keyboard binding moves Marty"));
        Key(EKeys::W, false); Next(); break;
    case 2:
        if (SmokeTime < .8) break;
        SmokeCheck(Marty->GetVelocity().Size2D() < 1, TEXT("Keyboard release stops Marty"));
        Key(EKeys::V, true); Next(); break;
    case 3:
        if (SmokeTime < .1) break;
        Key(EKeys::V, false); Next(); break;
    case 4:
        if (SmokeTime < .1) break;
        SmokeCheck(Marty->CameraPreset == 1, TEXT("V switches camera"));
        Key(EKeys::H, true); Next(); break;
    case 5:
        if (SmokeTime < .1) break;
        Key(EKeys::H, false); Next(); break;
    case 6:
        if (SmokeTime < .8) break;
        SmokeCheck(FVector::Dist2D(FVector(-200, -200, 0), Marty->GetActorLocation()) < 3, TEXT("H returns Marty home"));
        SmokePosition = Marty->GetActorLocation(); Next(); break;
    case 7:
        if (SmokeTime < 1) break;
        Axis(EKeys::Gamepad_LeftX, 0);
        SmokeCheck(FVector::Dist2D(SmokePosition, Marty->GetActorLocation()) > 20, TEXT("Synthetic left stick moves Marty"));
        Next(); break;
    case 8:
        Axis(EKeys::Gamepad_LeftX, 0);
        if (SmokeTime < .8) break;
        SmokeCheck(Marty->GetVelocity().Size2D() < 1, TEXT("Stick release stops Marty"));
        Key(EKeys::Gamepad_FaceButton_Bottom, true); Next(); break;
    case 9:
        if (SmokeTime < .1) break;
        SmokeCheck(Marty->GetCharacterMovement()->MaxWalkSpeed > 200, TEXT("A/Cross enables hurry"));
        Key(EKeys::Gamepad_FaceButton_Bottom, false); Key(EKeys::Gamepad_RightThumbstick, true); Next(); break;
    case 10:
        if (SmokeTime < .1) break;
        Key(EKeys::Gamepad_RightThumbstick, false); Next(); break;
    case 11:
        if (SmokeTime < .1) break;
        SmokeCheck(Marty->CameraPreset == 2, TEXT("R3 switches camera"));
        SmokeCheck(FMath::IsNearlyEqual(Marty->GetCharacterMovement()->MaxWalkSpeed, 100.f), TEXT("A/Cross release restores walking"));
        Key(EKeys::Gamepad_Special_Right, true); Next(); break;
    case 12:
        if (SmokeTime < .1) break;
        Key(EKeys::Gamepad_Special_Right, false); Next(); break;
    case 13:
        if (SmokeTime < .8) break;
        SmokeCheck(FVector::Dist2D(FVector(-200, -200, 0), Marty->GetActorLocation()) < 3, TEXT("Menu/Options returns Marty home"));
        if (FApp::CanEverRender())
            FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir() / TEXT("MovementGameplay.png"), true, false);
        Key(EKeys::S, true); Key(EKeys::LeftShift, true); Next(); break;
    case 14:
        if (SmokeTime < 6) break;
        Key(EKeys::S, false); Key(EKeys::LeftShift, false);
        SmokeCheck(Marty->GetActorLocation().X > -470 && Marty->GetActorLocation().Y > -470 &&
                   Marty->GetCharacterMovement()->IsMovingOnGround(), TEXT("Room boundaries block sustained movement without falling"));
        Next(); break;
    case 15:
        if (SmokeTime < 3) break;
        {
            auto Report = MakeShared<FJsonObject>();
            auto Strings = [](const TArray<FString>& Values)
            {
                TArray<TSharedPtr<FJsonValue>> Result;
                for (const auto& Value : Values) Result.Add(MakeShared<FJsonValueString>(Value));
                return Result;
            };
            Report->SetStringField(TEXT("engine"), FEngineVersion::Current().ToString());
            Report->SetStringField(TEXT("scope"), TEXT("Native gameplay with synthetic keyboard/gamepad events; physical hardware unverified"));
            Report->SetArrayField(TEXT("passed"), Strings(SmokePassed));
            Report->SetArrayField(TEXT("failed"), Strings(SmokeFailed));
            FString Json;
            FJsonSerializer::Serialize(Report, TJsonWriterFactory<>::Create(&Json));
            const bool Saved = FFileHelper::SaveStringToFile(Json, *(FPaths::ProjectSavedDir() / TEXT("movement-smoke.json")));
            FPlatformMisc::RequestExitWithStatus(false, Saved && SmokeFailed.IsEmpty() ? 0 : 1);
            Next();
        }
        break;
    }
}
