// Explicit opt-in native runtime acceptance. Synthetic device events are not physical hardware tests.
#include "MartyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
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
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetSmoke"))){TickRatchetSmoke(DeltaSeconds);return;}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCostumeSmoke"))){TickCostumeSmoke(DeltaSeconds);return;}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieWorldSmoke"))){TickWorldSmoke(DeltaSeconds);return;}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCompanionSmoke"))){TickCompanionSmoke(DeltaSeconds);return;}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieNavigationSmoke"))){TickNavigationSmoke(DeltaSeconds);return;}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieTricksSmoke"))) { TickTricksSmoke(DeltaSeconds); return; }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieAudioSmoke"))) { TickAudioSmoke(DeltaSeconds); return; }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieLifeSmoke"))) { TickLifeSmoke(DeltaSeconds); return; }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOutsideSmoke"))) { TickOutsideSmoke(DeltaSeconds); return; }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieGaitSmoke"))) { TickGaitSmoke(DeltaSeconds); return; }
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieAdventureSmoke"))) { TickAdventureSmoke(DeltaSeconds); return; }
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
    if (SmokeStage == 14) { Axis(EKeys::Gamepad_RightX, .7f); Axis(EKeys::Gamepad_RightY, .5f); }
    switch (SmokeStage)
    {
    case 0:
        // Give DX12 materials, Lumen and temporal history time to settle before capturing play.
        if (SmokeTime < (FApp::CanEverRender() ? 60.0 : 2.0)) break;
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
        SmokeCheck(Marty->CameraPreset == 1 && FMath::IsNearlyEqual(Marty->FindComponentByClass<USpringArmComponent>()->TargetArmLength, 330.f), TEXT("V switches camera distance"));
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
        SmokeCheck(FVector::Dist2D(SmokePosition, Marty->GetActorLocation()) > 20 && FVector::Dist2D(SmokePosition, Marty->GetActorLocation()) < 90, TEXT("Synthetic left stick preserves analog walking"));
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
        SmokeCheck(Marty->CameraPreset == 2 && FMath::IsNearlyEqual(Marty->FindComponentByClass<USpringArmComponent>()->TargetArmLength, 140.f), TEXT("R3 switches camera distance"));
        SmokeCheck(FMath::IsNearlyEqual(Marty->GetCharacterMovement()->MaxWalkSpeed, 100.f), TEXT("A/Cross release restores walking"));
        Key(EKeys::Gamepad_Special_Right, true); Next(); break;
    case 12:
        if (SmokeTime < .1) break;
        Key(EKeys::Gamepad_Special_Right, false); Next(); break;
    case 13:
        if (SmokeTime < .8) break;
        SmokeCheck(FVector::Dist2D(FVector(-200, -200, 0), Marty->GetActorLocation()) < 3, TEXT("Menu/Options returns Marty home"));
        SmokeYaw = PC->GetControlRotation().Yaw;
        SmokePitch = FRotator::NormalizeAxis(PC->GetControlRotation().Pitch); Next(); break;
    case 14:
        if (SmokeTime < .6) break;
        Axis(EKeys::Gamepad_RightX, 0);
        Axis(EKeys::Gamepad_RightY, 0);
        SmokeCheck(FMath::Abs(FRotator::NormalizeAxis(PC->GetControlRotation().Yaw - SmokeYaw)) > 5, TEXT("Right stick orbits camera"));
        SmokeCheck(FRotator::NormalizeAxis(PC->GetControlRotation().Pitch) > SmokePitch + 5, TEXT("Right stick up looks upward"));
        Key(EKeys::Gamepad_RightShoulder, true); Next(); break;
    case 15:
        Axis(EKeys::Gamepad_RightX, 0);
        Axis(EKeys::Gamepad_RightY, 0);
        if (SmokeTime < .1) break;
        Key(EKeys::Gamepad_RightShoulder, false); Next(); break;
    case 16:
        if (SmokeTime < .1) break;
        SmokeCheck(FMath::Abs(FRotator::NormalizeAxis(PC->GetControlRotation().Yaw - Marty->GetActorRotation().Yaw)) < 1 && FMath::Abs(FRotator::NormalizeAxis(PC->GetControlRotation().Pitch) + 12) < 1, TEXT("RB/R1 recenters camera"));
        if (FApp::CanEverRender())
            FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir() / TEXT("MovementGameplay.png"), true, false);
        Key(EKeys::S, true); Key(EKeys::LeftShift, true); Next(); break;
    case 17:
        if (SmokeTime < 6) break;
        Key(EKeys::S, false); Key(EKeys::LeftShift, false);
        SmokeCheck(Marty->GetActorLocation().X > -470 && Marty->GetActorLocation().X < -430 && Marty->GetActorLocation().Y > -470 &&
                   Marty->GetCharacterMovement()->IsMovingOnGround(), TEXT("Room boundaries block sustained movement without falling"));
        Next(); break;
    case 18:
        if (SmokeTime < 3) break;
        // Controlled fixture setup, followed by real movement input into the sofa.
        Marty->GetCharacterMovement()->StopMovementImmediately();
        Marty->TeleportTo(FVector(180, -100, 60), FRotator::ZeroRotator);
        PC->SetControlRotation(FRotator::ZeroRotator); Next(); break;
    case 19:
        if (SmokeTime < .8) break;
        Key(EKeys::W, true); Key(EKeys::LeftShift, true); Next(); break;
    case 20:
        if (SmokeTime < 1.5) break;
        Key(EKeys::W, false); Key(EKeys::LeftShift, false);
        UE_LOG(LogTemp, Display, TEXT("CHEWEENIE_SOFA_POSITION %s grounded=%d"), *Marty->GetActorLocation().ToString(), Marty->GetCharacterMovement()->IsMovingOnGround());
        SmokeCheck(Marty->GetActorLocation().X > 180 && Marty->GetActorLocation().X < 255 &&
                   Marty->GetCharacterMovement()->IsMovingOnGround(), TEXT("Sofa blocks actual character movement"));
        Marty->ReturnHome(); Next(); break;
    case 21:
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
