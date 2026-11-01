#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

bool AMartyCharacter::TrickModifierDown() const
{
    const auto* PC=Cast<APlayerController>(Controller);
    return PC&&(PC->IsInputKeyDown(EKeys::Gamepad_LeftShoulder)||PC->IsInputKeyDown(EKeys::LeftControl));
}
bool AMartyCharacter::TrickModifierVisible() const
{
    const auto* S=Adventure();
    return ModifierActive&&!ModifierBlocked&&TrickModifierDown()&&S&&S->Enabled&&S->Screen==ECozyScreen::Playing;
}
void AMartyCharacter::BeginTrickModifier()
{
    // A face action can arrive before the modifier action in the same input frame.
    // Initializing once keeps that chord from being mistaken for a later bark tap.
    if(ModifierActive)return;
    ModifierActive=true;ModifierUsed=false;ModifierStartedAt=FPlatformTime::Seconds();
    const auto* PC=Cast<APlayerController>(Controller);ModifierBarkTap=PC&&PC->IsInputKeyDown(EKeys::Gamepad_LeftShoulder);
    const auto* S=Adventure();ModifierBlocked=!S||!S->Enabled||S->Screen!=ECozyScreen::Playing;
    SetSprint(false);
}
void AMartyCharacter::EndTrickModifier()
{
    const auto* S=Adventure();const auto* PC=Cast<APlayerController>(Controller);
    const bool FaceHeld=PC&&(PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom)||PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Right)||PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Left)||PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Top));
    const bool Tap=ModifierBarkTap&&ModifierActive&&!ModifierUsed&&!ModifierBlocked&&!FaceHeld&&S&&S->Screen==ECozyScreen::Playing&&FPlatformTime::Seconds()-ModifierStartedAt<.3;
    CancelTrickModifier();if(Tap&&Life)Life->Bark();
}
void AMartyCharacter::CancelTrickModifier(){ModifierActive=false;ModifierUsed=false;ModifierBlocked=false;ModifierBarkTap=false;}
void AMartyCharacter::UpdateTrickModifier()
{
    if(!ModifierActive)return;
    // A menu/focus transition disarms the hold until release; it never queues a trick.
    const auto* S=Adventure();if(!S||S->Screen!=ECozyScreen::Playing){ModifierBlocked=true;ModifierUsed=true;}
    if(!TrickModifierDown())CancelTrickModifier();
}
bool AMartyCharacter::TryTrickShortcut(int32 Trick)
{
    if(!TrickModifierDown())return false;
    BeginTrickModifier();const auto* S=Adventure();
    if(!S||!S->Enabled||S->Screen!=ECozyScreen::Playing){ModifierBlocked=true;ModifierUsed=true;return false;}
    // Consume the face press even if a trick is disabled/busy, preventing base-action leaks.
    ModifierUsed=true;if(ModifierBlocked||!Life)return true;
    switch(Trick){case 0:Life->LieDown();break;case 1:Life->RollOver();break;case 2:Life->ShakePaw();break;case 3:Life->Bark();break;}
    return true;
}
void AMartyCharacter::ShortcutLie(){TryTrickShortcut(0);}
void AMartyCharacter::ShortcutRoll(){TryTrickShortcut(1);}
void AMartyCharacter::ShortcutShake(){TryTrickShortcut(2);}
void AMartyCharacter::ShortcutBark(){TryTrickShortcut(3);}
