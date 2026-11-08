#include "ControllerComfort.h"
#include "MartyCharacter.h"
#include "EnhancedPlayerInput.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/CoreDelegates.h"

namespace { const TCHAR* Section=TEXT("Cheweenie.ControllerComfort"); }
void UControllerComfort::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);ResetDefaults(false);
    GConfig->GetFloat(Section,TEXT("LeftDeadZone"),LeftDeadZone,GGameUserSettingsIni);
    GConfig->GetFloat(Section,TEXT("RightDeadZone"),RightDeadZone,GGameUserSettingsIni);
    GConfig->GetFloat(Section,TEXT("LookSpeed"),LookSpeed,GGameUserSettingsIni);
    GConfig->GetBool(Section,TEXT("PreciseLook"),PreciseLook,GGameUserSettingsIni);
    GConfig->GetInt(Section,TEXT("L3Mode"),L3Mode,GGameUserSettingsIni);
    LeftDeadZone=FMath::IsFinite(LeftDeadZone)?FMath::Clamp(LeftDeadZone,.15f,.35f):.2f;
    RightDeadZone=FMath::IsFinite(RightDeadZone)?FMath::Clamp(RightDeadZone,.15f,.35f):.2f;
    LookSpeed=FMath::IsFinite(LookSpeed)?FMath::Clamp(LookSpeed,.5f,2.f):1.f;L3Mode=FMath::Clamp(L3Mode,0,2);
    const float Defaults[]={220,330,140,120,400,170};
    for(int32 I=0;I<6;++I){GConfig->GetFloat(Section,*FString::Printf(TEXT("Zoom%d"),I),CameraDistances[I],GGameUserSettingsIni);CameraDistances[I]=FMath::IsFinite(CameraDistances[I])?FMath::Clamp(CameraDistances[I],80.f,650.f):Defaults[I];}
    FCoreDelegates::ApplicationWillDeactivateDelegate.AddUObject(this,&UControllerComfort::ClearLocalInput);
    IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddUObject(this,&UControllerComfort::DeviceConnectionChanged);
}
void UControllerComfort::Deinitialize()
{
    FCoreDelegates::ApplicationWillDeactivateDelegate.RemoveAll(this);
    IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().RemoveAll(this);
    Super::Deinitialize();
}
void UControllerComfort::ClearLocalInput()
{
    if(auto* World=GetGameInstance()->GetWorld())for(TActorIterator<AMartyCharacter> It(World);It;++It)if(It->IsLocallyControlled())It->ClearControllerInput();
}
void UControllerComfort::DeviceConnectionChanged(EInputDeviceConnectionState State,FPlatformUserId,FInputDeviceId)
{if(State==EInputDeviceConnectionState::Disconnected)ClearLocalInput();}
void UControllerComfort::Save()
{
    GConfig->SetFloat(Section,TEXT("LeftDeadZone"),LeftDeadZone,GGameUserSettingsIni);
    GConfig->SetFloat(Section,TEXT("RightDeadZone"),RightDeadZone,GGameUserSettingsIni);
    GConfig->SetFloat(Section,TEXT("LookSpeed"),LookSpeed,GGameUserSettingsIni);
    GConfig->SetBool(Section,TEXT("PreciseLook"),PreciseLook,GGameUserSettingsIni);
    GConfig->SetInt(Section,TEXT("L3Mode"),L3Mode,GGameUserSettingsIni);
    for(int32 I=0;I<6;++I)GConfig->SetFloat(Section,*FString::Printf(TEXT("Zoom%d"),I),CameraDistances[I],GGameUserSettingsIni);
    GConfig->Flush(false,GGameUserSettingsIni);
}
void UControllerComfort::ApplyPreset(bool Fast)
{LeftDeadZone=RightDeadZone=.2f;LookSpeed=Fast?1.5f:1.f;PreciseLook=!Fast;L3Mode=2;ClearLocalInput();Save();}
void UControllerComfort::ResetDefaults(bool Persist)
{
    LeftDeadZone=RightDeadZone=.2f;LookSpeed=1.f;PreciseLook=true;L3Mode=2;
    const float Defaults[]={220,330,140,120,400,170};for(int I=0;I<6;++I)CameraDistances[I]=Defaults[I];
    if(Persist){ClearLocalInput();ResetZoom();}
}
void UControllerComfort::ResetZoom(){const float D[]={220,330,140,120,400,170};for(int I=0;I<6;++I)CameraDistances[I]=D[I];if(auto* World=GetGameInstance()->GetWorld())for(TActorIterator<AMartyCharacter> It(World);It;++It)if(It->IsLocallyControlled())It->RefreshControllerZoom();Save();}
TArray<FString> UControllerComfort::Rows() const
{
    const bool Zones=FMath::IsNearlyEqual(LeftDeadZone,.2f)&&FMath::IsNearlyEqual(RightDeadZone,.2f)&&L3Mode==2;
    const TCHAR* Preset=Zones&&PreciseLook&&FMath::IsNearlyEqual(LookSpeed,1.f)?TEXT("Comfort"):Zones&&!PreciseLook&&FMath::IsNearlyEqual(LookSpeed,1.5f)?TEXT("Fast"):TEXT("Custom");
    const TCHAR* Modes[]={TEXT("off"),TEXT("hold"),TEXT("toggle")};
    return {FString(TEXT("Controller preset: "))+Preset,FString::Printf(TEXT("Left stick dead zone: %.1f%%"),LeftDeadZone*100),FString::Printf(TEXT("Right stick dead zone: %.1f%%"),RightDeadZone*100),FString::Printf(TEXT("Camera turn speed: %.0f%%"),LookSpeed*100),FString(TEXT("Camera response: "))+(PreciseLook?TEXT("precise"):TEXT("linear")),FString(TEXT("L3 hurry: "))+Modes[FMath::Clamp(L3Mode,0,2)],TEXT("Reset camera zoom distances"),TEXT("Restore controller defaults"),TEXT("Back")};
}
void UControllerComfort::Adjust(int32 Row,int32 Direction)
{
    if(!Direction)return;
    if(Row==0){ApplyPreset(!(PreciseLook==false&&FMath::IsNearlyEqual(LookSpeed,1.5f)));return;}
    if(Row==1)LeftDeadZone=FMath::Clamp(LeftDeadZone+(Direction>0?.025f:-.025f),.15f,.35f);
    else if(Row==2)RightDeadZone=FMath::Clamp(RightDeadZone+(Direction>0?.025f:-.025f),.15f,.35f);
    else if(Row==3)LookSpeed=FMath::Clamp(LookSpeed+(Direction>0?.1f:-.1f),.5f,2.f);
    else if(Row==4)PreciseLook=!PreciseLook;
    else if(Row==5){L3Mode=(L3Mode+(Direction>0?1:2))%3;ClearLocalInput();}
    else if(Row==6){ResetZoom();return;}
    else if(Row==7){ResetDefaults();return;}
    else return;
    Save();
}
float UControllerComfort::FilterAxis(float Raw,bool Right) const
{
    if(!FMath::IsFinite(Raw))return 0;
    const float Zone=Right?RightDeadZone:LeftDeadZone;
    return FMath::Clamp((FMath::Abs(Raw)-Zone)/(1-Zone),0.f,1.f)*FMath::Sign(Raw);
}
FVector2D UControllerComfort::ShapeLook(FVector2D Axis) const
{
    if(PreciseLook){Axis.X=FMath::Pow(FMath::Abs(Axis.X),1.35f)*FMath::Sign(Axis.X);Axis.Y=FMath::Pow(FMath::Abs(Axis.Y),1.35f)*FMath::Sign(Axis.Y);}
    return Axis*LookSpeed;
}
FInputActionValue UCozyStickDeadZone::ModifyRaw_Implementation(const UEnhancedPlayerInput* Input,FInputActionValue Value,float)
{
    auto* World=Input?Input->GetWorld():nullptr;
    auto* Prefs=World?World->GetGameInstance()->GetSubsystem<UControllerComfort>():nullptr;
    if(Input)if(auto* PC=Input->GetOuterAPlayerController())if(auto* Pawn=Cast<AMartyCharacter>(PC->GetPawn()))
    {
        const auto Raw=Value.Get<FVector>();Pawn->ObserveControllerStick(RightStick,FVector2D(Raw.X,Raw.Y));
        if(Pawn->ControllerAxisBlocked(RightStick))return FInputActionValue(Value.GetValueType(),FVector::ZeroVector);
    }
    auto Raw=Value.Get<FVector>();
    auto Filter=[&](float V){return Prefs?Prefs->FilterAxis(V,RightStick):FMath::Clamp((FMath::Abs(V)-.2f)/.8f,0.f,1.f)*FMath::Sign(V);};
    return FInputActionValue(Value.GetValueType(),FVector(Filter(Raw.X),Filter(Raw.Y),Filter(Raw.Z)));
}
