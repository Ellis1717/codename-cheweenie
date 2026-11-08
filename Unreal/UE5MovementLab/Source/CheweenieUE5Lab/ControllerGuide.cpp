#include "ControllerGuide.h"
#include "KeyboardGuide.h"
#include "ControllerComfort.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/InputDeviceRegistry.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "GameFramework/HUD.h"
#include "Misc/ConfigCacheIni.h"

class FCozyDeviceObserver final : public IInputProcessor
{
    TWeakObjectPtr<UControllerGuide> Guide;
public:
    explicit FCozyDeviceObserver(UControllerGuide* InGuide):Guide(InGuide){}
    virtual void Tick(float, FSlateApplication&, TSharedRef<ICursor>) override {}
    virtual bool HandleKeyDownEvent(FSlateApplication&,const FKeyEvent& E) override
    { if(Guide.IsValid()&&!E.IsCommandDown())Guide->Observe(E.GetKey(),E.GetInputDeviceId());return false; }
    virtual bool HandleAnalogInputEvent(FSlateApplication&,const FAnalogInputEvent& E) override
    { if(Guide.IsValid())Guide->Observe(E.GetKey(),E.GetInputDeviceId(),E.GetAnalogValue());return false; }
    virtual bool HandleMouseButtonDownEvent(FSlateApplication&,const FPointerEvent& E) override
    { if(Guide.IsValid())Guide->Observe(EKeys::LeftMouseButton,E.GetInputDeviceId());return false; }
    virtual bool HandleMouseMoveEvent(FSlateApplication&,const FPointerEvent& E) override
    { if(Guide.IsValid()&&E.GetCursorDelta().SizeSquared()>4)Guide->Observe(EKeys::MouseX,E.GetInputDeviceId());return false; }
};

void UControllerGuide::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    GConfig->GetInt(TEXT("Cheweenie.ControllerGuide"),TEXT("Style"),Override,GGameUserSettingsIni);
    Override=FMath::Clamp(Override,0,4);
    if(FSlateApplication::IsInitialized())
    {
        Processor=MakeShared<FCozyDeviceObserver>(this);
        FSlateApplication::Get().RegisterInputPreProcessor(Processor);
    }
}
void UControllerGuide::Deinitialize()
{
    if(Processor&&FSlateApplication::IsInitialized())FSlateApplication::Get().UnregisterInputPreProcessor(Processor);
    Processor.Reset();Super::Deinitialize();
}
ECozyInputFamily UControllerGuide::Classify(const FString& Name)
{
    if(Name.Contains(TEXT("DualSense"))||Name.Contains(TEXT("DualShock"))||Name.Contains(TEXT("PlayStation"))||Name.Contains(TEXT("Sony"))||Name.Contains(TEXT("PS4"))||Name.Contains(TEXT("PS5")))return ECozyInputFamily::PlayStation;
    if(Name.Contains(TEXT("XInput"))||Name.Contains(TEXT("Xbox"))||Name.Contains(TEXT("Wolverine Ultimate")))return ECozyInputFamily::Xbox;
    return ECozyInputFamily::Generic;
}
void UControllerGuide::Observe(FKey Key,FInputDeviceId Device,float Amount)
{
    // Unmapped keys (including Print Screen) and OS shortcuts are not gameplay input.
    if(!Key.IsValid()||Key==EKeys::LeftCommand||Key==EKeys::RightCommand)return;
    // Ignore resting stick noise and released trigger events.
    if(FMath::Abs(Amount)<.25f)return;
    if(!Key.IsGamepadKey()){Detected=ECozyInputFamily::Keyboard;return;}
    Detected=ECozyInputFamily::Generic;
    if(const auto D=FInputDeviceRegistry::FindDescriptor(Device))
        Detected=Classify(D->InputDeviceName.ToString()+TEXT(" ")+D->HardwareDeviceIdentifier.ToString());
}
ECozyInputFamily UControllerGuide::Family() const
{
    switch(Override){case 1:return ECozyInputFamily::Xbox;case 2:return ECozyInputFamily::PlayStation;case 3:return ECozyInputFamily::Keyboard;case 4:return ECozyInputFamily::Generic;default:return Detected;}
}
void UControllerGuide::CycleOverride()
{
    Override=(Override+1)%5;
    GConfig->SetInt(TEXT("Cheweenie.ControllerGuide"),TEXT("Style"),Override,GGameUserSettingsIni);
    GConfig->Flush(false,GGameUserSettingsIni);
}
FString UControllerGuide::Label() const
{
    switch(Family()){case ECozyInputFamily::Xbox:return TEXT("Xbox controller");case ECozyInputFamily::PlayStation:return TEXT("PlayStation controller");case ECozyInputFamily::Generic:return TEXT("Generic controller");default:return TEXT("Keyboard + mouse");}
}
FString UControllerGuide::PreferenceLabel() const
{
    return TEXT("Controller display: ")+ (Override==0?FString(TEXT("Auto")):Label())+TEXT(" (change)");
}
TArray<FString> UControllerGuide::Controls() const
{
    if(Family()==ECozyInputFamily::Keyboard)return CozyKeyboardControls(GetWorld());
    const bool PS=Family()==ECozyInputFamily::PlayStation;
    const int32 Mode=GetGameInstance()->GetSubsystem<UControllerComfort>()->L3Mode;
    const FString L3=Mode==2?TEXT(" | L3: toggle hurry"):Mode==1?TEXT(" | L3: hold hurry"):TEXT(" | L3: off");
    if(Family()==ECozyInputFamily::Keyboard)return {
        TEXT("WASD: move | Mouse: look | Shift: hurry"),TEXT("E: interact | Space: jump | B: bark"),TEXT("F: dig | Q: drop | Tab: backpack"),TEXT("V: camera | R: recenter | H: home | +/-: zoom"),TEXT("M: map | K: crew | C: trick menu"),TEXT("Hold Left Ctrl + 1/2/3/4: lie/roll/shake/bark"),TEXT("Arrows: menu | Enter: select | Backspace: back"),TEXT("Esc: pause | Z: crawl | U: two legs (solo Marty)")};
    if(Family()==ECozyInputFamily::Generic)return {
        FString(TEXT("LS: move/menu | RS: look | Bottom: hurry"))+L3,TEXT("Left face: interact | Top face: jump"),TEXT("Right face: trick menu | Tap LB: bark"),TEXT("RT: dig | LT: drop | Left special: bag"),TEXT("R3: camera | RB: recenter | Right special: pause"),TEXT("D-pad Up: map | Down: crew | Left/right: zoom"),TEXT("Hold LB + Bottom/Right/Left/Top: tricks"),TEXT("Hold LB + L3: crawl | + R3: two legs (solo Marty)")};
    return {
        FString(PS?TEXT("LS: move/menu | RS: look | Cross: hurry"):TEXT("LS: move/menu | RS: look | A: hurry"))+L3,
        PS?TEXT("Square: interact | Triangle: jump"):TEXT("X: interact | Y: jump"),
        PS?TEXT("Circle: trick menu | Tap L1: bark"):TEXT("B: trick menu | Tap LB: bark"),
        PS?TEXT("R2: dig | L2: drop | Touchpad: backpack"):TEXT("RT: dig | LT: drop | View: backpack"),
        PS?TEXT("R3: camera | R1: recenter | Options: pause"):TEXT("R3: camera | RB: recenter | Menu: pause"),
        TEXT("D-pad Up: map | Down: crew | Left/right: zoom"),
        PS?TEXT("Hold L1 + Cross/Circle/Square/Triangle: tricks"):TEXT("Hold LB + A/B/X/Y: lie/roll/shake/bark"),
        PS?TEXT("Menus: LS / D-pad | Cross: select | Circle: back"):TEXT("Menus: LS / D-pad | A: select | B: back"),
        PS?TEXT("Hold L1 + L3: crawl | + R3: two legs (solo Marty)"):TEXT("Hold LB + L3: crawl | + R3: two legs (solo Marty)")};
}
FString UControllerGuide::Format(const FString& Text) const
{
    FString Result;
    const int32 Column=Family()==ECozyInputFamily::Keyboard?1:Family()==ECozyInputFamily::Xbox?2:Family()==ECozyInputFamily::PlayStation?3:4;
    // Match complete input phrases before individual glyph names. Leave ordinary
    // slashes (scores, place names and menu titles) untouched.
    const TCHAR* Phrases[][5]={
        {TEXT("V / R3: six views | Wheel / +/-: zoom | Backslash: reset zoom"),TEXT("V: six views | Wheel / +/-: zoom | Backslash: reset zoom"),TEXT("R3: six views | D-pad: zoom (hold RB in rooms)"),TEXT("R3: six views | D-pad: zoom (hold R1 in rooms)"),TEXT("Right-stick click: six views | D-pad: zoom")},
        {TEXT("brackets / D-pad left-right"),TEXT("brackets"),TEXT("D-pad left/right"),TEXT("D-pad left/right"),TEXT("D-pad left/right")},
        {TEXT("Wheel / +/-: zoom | Z: reset zoom"),TEXT("Wheel / +/-: zoom | Z: reset zoom"),TEXT("Zoom: Controls & cameras"),TEXT("Zoom: Controls & cameras"),TEXT("Zoom: Controls & cameras")},
        {TEXT("Arrows choose | Enter select | Backspace back"),TEXT("Arrows choose | Enter select | Backspace back"),TEXT("LS/D-pad choose | A select | B back"),TEXT("LS/D-pad choose | Cross select | Circle back"),TEXT("LS/D-pad choose | Bottom select | Right back")},
        {TEXT("C: trick menu"),TEXT("C: trick menu"),TEXT("B: trick menu"),TEXT("Circle: trick menu"),TEXT("right face: trick menu")},
        {TEXT("Keyboard: 1 lie / stand | 2 roll | 3 shake | 4 bark"),TEXT("Keyboard: 1 lie / stand | 2 roll | 3 shake | 4 bark"),TEXT("Hold LB while choosing a trick"),TEXT("Hold L1 while choosing a trick"),TEXT("Hold LB while choosing a trick")},
        {TEXT("WASD / LS"),TEXT("WASD"),TEXT("LS"),TEXT("LS"),TEXT("LS")},
        {TEXT("mouse / RS"),TEXT("mouse"),TEXT("RS"),TEXT("RS"),TEXT("RS")},
        {TEXT("B / LB-L1"),TEXT("B"),TEXT("LB"),TEXT("L1"),TEXT("LB")},
        {TEXT("R / RB-R1"),TEXT("R"),TEXT("RB"),TEXT("R1"),TEXT("RB")},
        {TEXT("Z / LB-L3"),TEXT("Z"),TEXT("LB + L3"),TEXT("L1 + L3"),TEXT("LB + L3")},
        {TEXT("U / LB-R3"),TEXT("U"),TEXT("LB + R3"),TEXT("L1 + R3"),TEXT("LB + R3")},
        {TEXT("WASD / left stick"),TEXT("WASD"),TEXT("left stick"),TEXT("left stick"),TEXT("left stick")},
        {TEXT("mouse / right stick"),TEXT("mouse"),TEXT("right stick"),TEXT("right stick"),TEXT("right stick")},
        {TEXT("E / X-Square"),TEXT("E"),TEXT("X"),TEXT("Square"),TEXT("left face")},
        {TEXT("Space / Y-Triangle"),TEXT("Space"),TEXT("Y"),TEXT("Triangle"),TEXT("top face")},
        {TEXT("Shift / A-Cross"),TEXT("Shift"),TEXT("A"),TEXT("Cross"),TEXT("bottom face")},
        {TEXT("Tab / View-Touchpad"),TEXT("Tab"),TEXT("View"),TEXT("Touchpad"),TEXT("left special")},
        {TEXT("Tab / View"),TEXT("Tab"),TEXT("View"),TEXT("Touchpad"),TEXT("left special")},
        {TEXT("F / RT-R2"),TEXT("F"),TEXT("RT"),TEXT("R2"),TEXT("RT")},
        {TEXT("F / RT"),TEXT("F"),TEXT("RT"),TEXT("R2"),TEXT("RT")},
        {TEXT("Q / LT"),TEXT("Q"),TEXT("LT"),TEXT("L2"),TEXT("LT")},
        {TEXT("V / R3"),TEXT("V"),TEXT("R3"),TEXT("R3"),TEXT("R3")},
        {TEXT("R / RB"),TEXT("R"),TEXT("RB"),TEXT("R1"),TEXT("RB")},
        {TEXT("B / tap LB"),TEXT("B"),TEXT("tap LB"),TEXT("tap L1"),TEXT("tap LB")},
        {TEXT("M / D-pad Up"),TEXT("M"),TEXT("D-pad Up"),TEXT("D-pad Up"),TEXT("D-pad Up")},
        {TEXT("K / D-pad Down"),TEXT("K"),TEXT("D-pad Down"),TEXT("D-pad Down"),TEXT("D-pad Down")},
        {TEXT("C / B-Circle"),TEXT("C"),TEXT("B"),TEXT("Circle"),TEXT("right face")},
        {TEXT("Esc / Options"),TEXT("Esc"),TEXT("Menu"),TEXT("Options"),TEXT("right special")},
        {TEXT("Arrows / D-pad"),TEXT("Arrows"),TEXT("LS / D-pad"),TEXT("LS / D-pad"),TEXT("LS / D-pad")},
        {TEXT("Enter / A-Cross"),TEXT("Enter"),TEXT("A"),TEXT("Cross"),TEXT("bottom face")},
        {TEXT("L1 / LB or Left Ctrl"),TEXT("Left Ctrl"),TEXT("LB"),TEXT("L1"),TEXT("LB")},
        {TEXT("Cross / A"),TEXT("1"),TEXT("A"),TEXT("Cross"),TEXT("bottom face")},
        {TEXT("Circle / B"),TEXT("2"),TEXT("B"),TEXT("Circle"),TEXT("right face")},
        {TEXT("Square / X"),TEXT("3"),TEXT("X"),TEXT("Square"),TEXT("left face")},
        {TEXT("Triangle / Y"),TEXT("4"),TEXT("Y"),TEXT("Triangle"),TEXT("top face")},
        {TEXT("A-Cross"),TEXT("Enter"),TEXT("A"),TEXT("Cross"),TEXT("bottom face")},
        {TEXT("X-Square"),TEXT("E"),TEXT("X"),TEXT("Square"),TEXT("left face")},
        {TEXT("Y-Triangle"),TEXT("Space"),TEXT("Y"),TEXT("Triangle"),TEXT("top face")},
        {TEXT("B-Circle"),TEXT("Backspace"),TEXT("B"),TEXT("Circle"),TEXT("right face")},
        {TEXT("L1/LB"),TEXT("Left Ctrl"),TEXT("LB"),TEXT("L1"),TEXT("LB")},
        {TEXT("D-pad choose | A select | B back"),TEXT("Arrows choose | Enter select | Backspace back"),TEXT("LS/D-pad choose | A select | B back"),TEXT("LS/D-pad choose | Cross select | Circle back"),TEXT("LS/D-pad choose | Bottom select | Right back")}
    };
    // Scan the original text once; a replacement must never become another match.
    for(int32 At=0;At<Text.Len();)
    {
        bool Matched=false;
        for(const auto& Phrase:Phrases)
        {
            const int32 Length=FCString::Strlen(Phrase[0]);
            if(At+Length<=Text.Len()&&FCString::Strncmp(*Text+At,Phrase[0],Length)==0)
            {
                Result+=Phrase[Column];At+=Length;Matched=true;break;
            }
        }
        if(!Matched)Result.AppendChar(Text[At++]);
    }
    return Result;
}
void UControllerGuide::Draw(AHUD* HUD,float X,float Y,float Scale) const
{
    const auto White=FLinearColor(.9f,.94f,1),Edge=FLinearColor(.5f,.7f,.8f);
    auto Text=[&](const FString& T,float A,float B){HUD->DrawText(T,White,X+A*Scale,Y+B*Scale,nullptr,.9f*Scale);};
    auto Line=[&](float A,float B,float C,float D){HUD->DrawLine(X+A*Scale,Y+B*Scale,X+C*Scale,Y+D*Scale,Edge,3*Scale);};
    auto Ring=[&](float A,float B,float R){for(int I=0;I<32;++I){float T=I*2*PI/32,U=(I+1)*2*PI/32;Line(A+R*FMath::Cos(T),B+R*FMath::Sin(T),A+R*FMath::Cos(U),B+R*FMath::Sin(U));}};
    if(Family()==ECozyInputFamily::Keyboard)
    {
        CozyDrawKeyboard(HUD,X,Y,Scale);return;
    }
    Text(Label(),0,0);
    const bool PS=Family()==ECozyInputFamily::PlayStation,Generic=Family()==ECozyInputFamily::Generic;
    // Original line illustration, drawn at native resolution; no external artwork.
    const FVector2D Outline[]={{55,49},{110,40},{135,58},{225,58},{250,40},{305,49},{335,163},{312,182},{269,136},{91,136},{48,182},{25,163},{55,49}};
    for(int I=1;I<UE_ARRAY_COUNT(Outline);++I)Line(Outline[I-1].X,Outline[I-1].Y,Outline[I].X,Outline[I].Y);
    Ring(PS?112:90,PS?122:82,19);Text(TEXT("LS"),PS?104:82,PS?115:75);
    Ring(229,122,19);Text(TEXT("RS"),221,115);
    const float DX=PS?82:133,DY=PS?80:120;Line(DX-14,DY,DX+14,DY);Line(DX,DY-14,DX,DY+14);
    const TCHAR* Face[4]={Generic?TEXT("T"):TEXT("Y"),Generic?TEXT("L"):TEXT("X"),Generic?TEXT("R"):TEXT("B"),Generic?TEXT("B"):TEXT("A")};
    const FVector2D Buttons[]={{280,62},{255,86},{305,86},{280,110}};
    for(int I=0;I<4;++I)
    {
        const float A=Buttons[I].X,B=Buttons[I].Y;Ring(A,B,11);
        if(!PS){Text(Face[I],A-4,B-5);continue;}
        if(I==0){Line(A,B-6,A-6,B+5);Line(A-6,B+5,A+6,B+5);Line(A+6,B+5,A,B-6);}
        else if(I==1){Line(A-5,B-5,A+5,B-5);Line(A+5,B-5,A+5,B+5);Line(A+5,B+5,A-5,B+5);Line(A-5,B+5,A-5,B-5);}
        else if(I==2)Ring(A,B,6);
        else {Line(A-5,B-5,A+5,B+5);Line(A+5,B-5,A-5,B+5);}
    }
    Text(PS?TEXT("L2 / L1"):TEXT("LT / LB"),40,24);Text(PS?TEXT("R2 / R1"):TEXT("RT / RB"),265,24);
    Text(PS?TEXT("Touchpad"):Generic?TEXT("Special L"):TEXT("View"),145,75);Text(PS?TEXT("Options"):Generic?TEXT("Special R"):TEXT("Menu"),170,100);
}
