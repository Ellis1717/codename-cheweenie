#include "MartyCharacter.h"
#include "ControllerComfort.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputModifiers.h"

bool AMartyCharacter::CanAdjustCamera() const
{
    auto* PC=Cast<APlayerController>(Controller);
    return PC&&PC->GetViewTarget()==this&&!ControllerMenuOpen()&&CanPlay()&&(!Adventure()||Adventure()->Feature(ECozyFeature::Cameras));
}
FString AMartyCharacter::CameraName() const
{
    const TCHAR* Names[]={TEXT("Cozy follow"),TEXT("Wide follow"),TEXT("Close follow"),TEXT("Marty portrait"),TEXT("High overview"),TEXT("Dog level")};
    return Names[FMath::Clamp(CameraPreset,0,5)];
}
void AMartyCharacter::ResetCameraZoom()
{
    if(!CanAdjustCamera())return;
    const float Defaults[]={220,330,140,120,400,170};auto* Pref=GetGameInstance()->GetSubsystem<UControllerComfort>();
    Pref->CameraDistances[FMath::Clamp(CameraPreset,0,5)]=Defaults[FMath::Clamp(CameraPreset,0,5)];Pref->Save();RefreshControllerZoom();
}
void AMartyCharacter::ZoomWheel(const FInputActionValue& Value){ZoomCamera(Value.Get<float>()*30.f);}
void AMartyCharacter::ZoomHeld(const FInputActionValue& Value){ZoomCamera(Value.Get<float>()*180.f*GetWorld()->GetDeltaSeconds());}
void AMartyCharacter::SetupCameraZoom(UEnhancedInputComponent* Input,UInputMappingContext* Context)
{
    // Separate actions preserve both notches when direction changes within one input frame.
    for(FKey K:{EKeys::MouseScrollUp,EKeys::MouseScrollDown})
    {
        auto* Wheel=NewObject<UInputAction>(this);Wheel->ValueType=EInputActionValueType::Boolean;ZoomActions.Add(Wheel);Context->MapKey(Wheel,K);
        Input->BindAction(Wheel,ETriggerEvent::Started,this,K==EKeys::MouseScrollUp?&AMartyCharacter::ZoomIn:&AMartyCharacter::ZoomOut);
    }
    auto* Held=NewObject<UInputAction>(this);Held->ValueType=EInputActionValueType::Axis1D;ZoomActions.Add(Held);
    for(FKey K:{EKeys::Equals,EKeys::Add,EKeys::Hyphen,EKeys::Subtract})
    {
        auto& Map=Context->MapKey(Held,K);if(K==EKeys::Hyphen||K==EKeys::Subtract)Map.Modifiers.Add(NewObject<UInputModifierNegate>(Context));
    }
    Input->BindAction(Held,ETriggerEvent::Triggered,this,&AMartyCharacter::ZoomHeld);
    auto* Reset=NewObject<UInputAction>(this);Reset->ValueType=EInputActionValueType::Boolean;ZoomActions.Add(Reset);
    Context->MapKey(Reset,EKeys::Backslash);Input->BindAction(Reset,ETriggerEvent::Started,this,&AMartyCharacter::ResetCameraZoom);
}
