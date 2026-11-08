#include "KeyboardGuide.h"
#include "EnhancedPlayerInput.h"
#include "InputMappingContext.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

const TArray<FCozyKeyboardCap>& CozyKeyboardLayout()
{
    static const TArray<FCozyKeyboardCap> Caps=[]()
    {
        TArray<FCozyKeyboardCap> R;
        auto Key=[&](const TCHAR* Name,const TCHAR* Label,float X,float Y,float W=1.f,float H=1.f)
        {R.Add({FKey(FName(Name)),Label,X*31,Y,W*31-3,H*26-3});};
        Key(TEXT("Escape"),TEXT("Esc"),0,30);
        for(int I=1;I<=12;++I){const FString F=FString::Printf(TEXT("F%d"),I);Key(*F,*F,1.5f+I+(I>4?.25f:0)+(I>8?.25f:0),30);}
        Key(TEXT("PrintScreen"),TEXT("PrtSc"),16,30);Key(TEXT("ScrollLock"),TEXT("ScrLk"),17,30);Key(TEXT("Pause"),TEXT("Pause"),18,30);
        const TCHAR* Numbers[]={TEXT("Tilde"),TEXT("One"),TEXT("Two"),TEXT("Three"),TEXT("Four"),TEXT("Five"),TEXT("Six"),TEXT("Seven"),TEXT("Eight"),TEXT("Nine"),TEXT("Zero"),TEXT("Hyphen"),TEXT("Equals")};
        const TCHAR* NumberLabels[]={TEXT("`"),TEXT("1"),TEXT("2"),TEXT("3"),TEXT("4"),TEXT("5"),TEXT("6"),TEXT("7"),TEXT("8"),TEXT("9"),TEXT("0"),TEXT("-"),TEXT("= +")};
        for(int I=0;I<13;++I)Key(Numbers[I],NumberLabels[I],I,66);
        Key(TEXT("BackSpace"),TEXT("Backspace"),13,66,2);
        Key(TEXT("Tab"),TEXT("Tab"),0,92,1.5f);
        const TCHAR* Top[]={TEXT("Q"),TEXT("W"),TEXT("E"),TEXT("R"),TEXT("T"),TEXT("Y"),TEXT("U"),TEXT("I"),TEXT("O"),TEXT("P"),TEXT("LeftBracket"),TEXT("RightBracket")};
        for(int I=0;I<12;++I)Key(Top[I],I==10?TEXT("["):I==11?TEXT("]"):Top[I],1.5f+I,92);
        Key(TEXT("Backslash"),TEXT("\\"),13.5f,92,1.5f);
        Key(TEXT("CapsLock"),TEXT("Caps"),0,118,1.75f);
        const TCHAR* Home[]={TEXT("A"),TEXT("S"),TEXT("D"),TEXT("F"),TEXT("G"),TEXT("H"),TEXT("J"),TEXT("K"),TEXT("L"),TEXT("Semicolon"),TEXT("Apostrophe")};
        for(int I=0;I<11;++I)Key(Home[I],I==9?TEXT(";"):I==10?TEXT("'"):Home[I],1.75f+I,118);
        Key(TEXT("Enter"),TEXT("Enter"),12.75f,118,2.25f);
        Key(TEXT("LeftShift"),TEXT("L Shift"),0,144,2.25f);
        const TCHAR* Bottom[]={TEXT("Z"),TEXT("X"),TEXT("C"),TEXT("V"),TEXT("B"),TEXT("N"),TEXT("M"),TEXT("Comma"),TEXT("Period"),TEXT("Slash")};
        for(int I=0;I<10;++I)Key(Bottom[I],I==7?TEXT(","):I==8?TEXT("."):I==9?TEXT("/"):Bottom[I],2.25f+I,144);
        Key(TEXT("RightShift"),TEXT("R Shift"),12.25f,144,2.75f);
        Key(TEXT("LeftControl"),TEXT("L Ctrl"),0,170,1.5f);Key(TEXT("LeftCommand"),TEXT("Win"),1.5f,170,1.25f);Key(TEXT("LeftAlt"),TEXT("Alt"),2.75f,170,1.25f);
        Key(TEXT("SpaceBar"),TEXT("Space"),4,170,6.25f);Key(TEXT("RightAlt"),TEXT("Alt"),10.25f,170,1.25f);Key(TEXT("RightCommand"),TEXT("Win"),11.5f,170,1.25f);Key(TEXT("Menu"),TEXT("Menu"),12.75f,170,1);Key(TEXT("RightControl"),TEXT("Ctrl"),13.75f,170,1.25f);
        const TCHAR* Nav[]={TEXT("Insert"),TEXT("Home"),TEXT("PageUp"),TEXT("Delete"),TEXT("End"),TEXT("PageDown")};
        const TCHAR* NavLabels[]={TEXT("Ins"),TEXT("Home"),TEXT("PgUp"),TEXT("Del"),TEXT("End"),TEXT("PgDn")};
        for(int I=0;I<6;++I)Key(Nav[I],NavLabels[I],16+I%3,66+(I/3)*26);
        Key(TEXT("Up"),TEXT("Up"),17,144);Key(TEXT("Left"),TEXT("Left"),16,170);Key(TEXT("Down"),TEXT("Down"),17,170);Key(TEXT("Right"),TEXT("Right"),18,170);
        Key(TEXT("NumLock"),TEXT("Num"),20,66);Key(TEXT("Divide"),TEXT("/"),21,66);Key(TEXT("Multiply"),TEXT("*"),22,66);Key(TEXT("Subtract"),TEXT("-"),23,66);
        const TCHAR* Pad[]={TEXT("NumPadSeven"),TEXT("NumPadEight"),TEXT("NumPadNine"),TEXT("NumPadFour"),TEXT("NumPadFive"),TEXT("NumPadSix"),TEXT("NumPadOne"),TEXT("NumPadTwo"),TEXT("NumPadThree")};
        const TCHAR* PadLabels[]={TEXT("7"),TEXT("8"),TEXT("9"),TEXT("4"),TEXT("5"),TEXT("6"),TEXT("1"),TEXT("2"),TEXT("3")};
        for(int I=0;I<9;++I)Key(Pad[I],PadLabels[I],20+I%3,92+(I/3)*26);
        Key(TEXT("Add"),TEXT("+"),23,92,1,2);Key(TEXT("Enter"),TEXT("Enter"),23,144,1,2);
        Key(TEXT("NumPadZero"),TEXT("0"),20,170,2);Key(TEXT("Decimal"),TEXT("."),22,170);
        return R;
    }();
    return Caps;
}
TSet<FKey> CozyActiveKeyboardKeys(UWorld* World)
{
    TSet<FKey> Keys;
    const auto* PC=World?World->GetFirstPlayerController():nullptr;
    if(const auto* Input=PC?Cast<UEnhancedPlayerInput>(PC->PlayerInput):nullptr)
        for(const auto& Mapping:Input->GetEnhancedActionMappingsView())
            if(!Mapping.Key.IsGamepadKey())Keys.Add(Mapping.Key);
    return Keys;
}
void CozyDrawKeyboard(AHUD* HUD,float X,float Y,float Scale)
{
    const auto Active=CozyActiveKeyboardKeys(HUD->GetWorld());
    const FLinearColor Ink(.99f,.94f,.81f),Gold(.9f,.65f,.25f),Muted(.15f,.20f,.24f);
    auto Text=[&](const FString& T,float A,float B,float Size=.85f){HUD->DrawText(T,Ink,X+A*Scale,Y+B*Scale,nullptr,Size*Scale);};
    auto Rect=[&](FLinearColor C,float A,float B,float W,float H){HUD->DrawRect(C,X+A*Scale,Y+B*Scale,W*Scale,H*Scale);};
    auto Line=[&](float A,float B,float C,float D){HUD->DrawLine(X+A*Scale,Y+B*Scale,X+C*Scale,Y+D*Scale,Gold,2*Scale);};
    Text(TEXT("Keyboard + mouse | Gold: bound in this mode | Grey: unused"),0,0,.95f);
    for(const auto& Cap:CozyKeyboardLayout())
    {
        const bool Used=Active.Contains(Cap.Key);const float A=X+Cap.X*Scale,B=Y+Cap.Y*Scale;
        Rect(Used?Gold:Muted,Cap.X,Cap.Y,Cap.Width,Cap.Height);
        Rect(Used?FLinearColor(.32f,.23f,.11f):FLinearColor(.065f,.09f,.11f),Cap.X+1,Cap.Y+1,Cap.Width-2,Cap.Height-2);
        float W,H;const float FontScale=(Cap.Label.Len()>4?.65f:.8f)*Scale;HUD->GetTextSize(Cap.Label,W,H,nullptr,FontScale);
        HUD->DrawText(Cap.Label,Used?Ink:FLinearColor(.56f,.61f,.65f),A+(Cap.Width*Scale-W)*.5f,B+(Cap.Height*Scale-H)*.5f,nullptr,FontScale);
    }
    Rect(Muted,795,65,70,103);Rect(FLinearColor(.065f,.09f,.11f),797,67,66,99);
    Line(830,65,830,105);Line(795,105,865,105);Rect(Gold,825,77,10,20);
    Text(TEXT("L"),807,77,.9f);Text(TEXT("R"),845,77,.9f);Text(TEXT("Mouse"),808,132,.9f);
    Text(TEXT("Move: look"),877,75);Text(TEXT("Click: menu"),877,97);
    Text(Active.Contains(EKeys::MouseScrollUp)?TEXT("Wheel: zoom"):TEXT("Wheel: unused"),877,119);
}
TArray<FString> CozyKeyboardControls(UWorld* World)
{
    const auto Active=CozyActiveKeyboardKeys(World);
    const bool Room=Active.Contains(EKeys::F9);
    return {
        Room?TEXT("W A S D: move | Mouse: look"):TEXT("W A S D / arrow keys: move | Mouse: look"),
        TEXT("Left Shift: hurry | Space: jump | E: interact"),
        Room?TEXT("B: voice | Q: drop toy | H: return home"):TEXT("F: dig | Q: drop toy | B: bark"),
        Room?TEXT("1: Marty | 2: Lakitu | 3: Rio | 4: Ratchet"):TEXT("Tab: backpack | M: map | K: cat crew"),
        Room?TEXT("[ / ]: previous / next pet | T: costume"):TEXT("C: trick menu / back | H: return home"),
        Room?TEXT("F9: return to patrol"):TEXT("Hold Left Ctrl + 1/2/3/4: lie/roll/shake/bark"),
        TEXT("V: camera view | R: recenter camera"),
        Active.Contains(EKeys::Equals)?TEXT("Wheel / = (+) / numpad +: zoom in"):TEXT("Mouse: move to look | Left click: menu selection"),
        Active.Contains(EKeys::Hyphen)?TEXT("Wheel / - / numpad -: zoom out | Backslash: reset zoom"):TEXT("Camera controls depend on the selected mode"),
        TEXT("Arrows: choose / adjust menu | Enter: select"),
        Room?TEXT("Backspace: back | Esc: pause / modes"):TEXT("Backspace / C: back | Esc: pause"),
        Room?TEXT("Grey keys have no binding in this mode"):TEXT("Z: crawl | U: two legs | Grey keys: unused")};
}
