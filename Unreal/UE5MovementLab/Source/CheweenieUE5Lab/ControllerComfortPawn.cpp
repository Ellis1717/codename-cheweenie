#include "MartyCharacter.h"
#include "ControllerComfort.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "CozyModeHub.h"
#include "DogLife.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "UnrealClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Misc/App.h"

bool AMartyCharacter::ControllerMenuOpen() const
{
    if(const auto* PC=Cast<ACozyCoopController>(Controller))return PC->MenuOpen;
    const auto* S=Adventure();return S&&S->Enabled&&S->Screen!=ECozyScreen::Playing;
}
bool AMartyCharacter::ControllerAxisBlocked(bool Right) const
{return ControllerMenuOpen()||(Right?BlockRightUntilNeutral:BlockLeftUntilNeutral);}
void AMartyCharacter::ObserveControllerStick(bool Right,FVector2D Raw)
{
    (Right?RawControllerRight:RawControllerLeft)=Raw;
    if(FMath::Max(FMath::Abs(Raw.X),FMath::Abs(Raw.Y))<.3f)(Right?BlockRightUntilNeutral:BlockLeftUntilNeutral)=false;
}
void AMartyCharacter::ApplyControllerHurry(bool Active)
{
    if(Life)Life->SprintRequested=Active;
    GetCharacterMovement()->MaxWalkSpeed=Life?Life->WalkSpeed():Active?230:100;
}
void AMartyCharacter::RefreshControllerHurry()
{
    const bool Active=(HurryHeld||HurryL3Held||HurryLatched)&&!SuppressHurryUntilRelease&&!ControllerMenuOpen()&&CanPlay()&&!TrickModifierDown();
    if(Active!=LastHurryApplied){LastHurryApplied=Active;ApplyControllerHurry(Active);}
}
void AMartyCharacter::SetControllerHurryHeld(bool Active)
{
    if(!Active){HurryHeld=false;SuppressHurryUntilRelease=false;}
    else HurryHeld=CanPlay()&&!ControllerMenuOpen()&&!SuppressHurryUntilRelease&&!TrickModifierDown();
    RefreshControllerHurry();
}
void AMartyCharacter::ControllerHurryPress()
{
    if(ControllerMenuOpen()||!CanPlay()||TrickModifierDown())return;
    const auto* S=Adventure();if(S&&!S->Feature(ECozyFeature::Hurry))return;
    const int32 Mode=GetGameInstance()->GetSubsystem<UControllerComfort>()->L3Mode;
    if(Mode==2)HurryLatched=!HurryLatched;else if(Mode==1)HurryL3Held=true;
    RefreshControllerHurry();
}
void AMartyCharacter::ControllerHurryRelease(){HurryL3Held=false;RefreshControllerHurry();}
void AMartyCharacter::ClearControllerHurry(){HurryHeld=HurryL3Held=HurryLatched=false;RefreshControllerHurry();}
void AMartyCharacter::ClearControllerInput()
{
    ClearControllerHurry();
    if(const auto* PC=Cast<APlayerController>(Controller))
    {
        SuppressHurryUntilRelease=PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom)||PC->IsInputKeyDown(EKeys::LeftShift);
        BlockLeftUntilNeutral=FMath::Max(FMath::Abs(RawControllerLeft.X),FMath::Abs(RawControllerLeft.Y))>.3f;
        BlockRightUntilNeutral=FMath::Max(FMath::Abs(RawControllerRight.X),FMath::Abs(RawControllerRight.Y))>.3f;
    }
    WaitMenuNeutral=true;StickMenuX=StickMenuY=0;StickRepeat=0;
    if(Life)Life->ClearBodyStick();RefreshControllerHurry();
}
void AMartyCharacter::TickControllerComfort(float Delta)
{
    if(!IsLocallyControlled())return;
    auto* PC=Cast<APlayerController>(Controller);if(!PC)return;
    const FVector2D Left=RawControllerLeft;
    const bool NeutralLeft=FMath::Max(FMath::Abs(Left.X),FMath::Abs(Left.Y))<.3f;
    const bool Menu=ControllerMenuOpen();
    if(Menu!=HadControllerMenu){ClearControllerInput();HadControllerMenu=Menu;}
    if(!FApp::IsUnattended()&&GEngine&&GEngine->GameViewport&&GEngine->GameViewport->Viewport&&!GEngine->GameViewport->Viewport->HasFocus()&&(HurryLatched||HurryL3Held))ClearControllerInput();
    RefreshControllerHurry();
    if(ZoomInterpolating){Boom->TargetArmLength=FMath::FInterpTo(Boom->TargetArmLength,ZoomTarget,Delta,12.f);if(FMath::IsNearlyEqual(Boom->TargetArmLength,ZoomTarget,.1f)){Boom->TargetArmLength=ZoomTarget;ZoomInterpolating=false;}}
    if(!Menu)return;
    if(NeutralLeft){WaitMenuNeutral=false;StickMenuX=StickMenuY=0;StickRepeat=0;return;}
    if(WaitMenuNeutral)return;
    int32 X=0,Y=0;
    if(FMath::Abs(Left.Y)>=FMath::Abs(Left.X)&&FMath::Abs(Left.Y)>=.65f)Y=Left.Y>0?-1:1;
    else if(FMath::Abs(Left.X)>=.65f)X=Left.X>0?1:-1;
    else return;
    StickRepeat-=Delta;
    const bool Changed=X!=StickMenuX||Y!=StickMenuY;
    if(!Changed&&(X!=0||StickRepeat>0))return;
    StickMenuX=X;StickMenuY=Y;StickRepeat=Changed?.4f:.16f;
    if(auto* Room=Cast<ACozyCoopController>(Controller))
    {
        auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();if(Y)Hub->Navigate(Y);else Hub->Adjust(X);
    }
    else if(auto* S=Adventure()){if(Y)S->Navigate(Y);else S->Adjust(X);}
}
void AMartyCharacter::RefreshControllerZoom()
{ZoomTarget=GetGameInstance()->GetSubsystem<UControllerComfort>()->CameraDistances[FMath::Clamp(CameraPreset,0,3)];ZoomInterpolating=true;}
void AMartyCharacter::ZoomCamera(float Amount)
{
    if(ControllerMenuOpen()||!CanPlay()||(Adventure()&&!Adventure()->Feature(ECozyFeature::Cameras))||!FMath::IsFinite(Amount))return;
    auto* PC=Cast<APlayerController>(Controller);if(!PC||PC->GetViewTarget()!=this)return;
    ZoomTarget=FMath::Clamp((ZoomInterpolating?ZoomTarget:Boom->TargetArmLength)-Amount,80.f,650.f);
    auto* P=GetGameInstance()->GetSubsystem<UControllerComfort>();P->CameraDistances[FMath::Clamp(CameraPreset,0,3)]=ZoomTarget;P->Save();ZoomInterpolating=true;
}
