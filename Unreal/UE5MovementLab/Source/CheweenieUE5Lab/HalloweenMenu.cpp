#include "CozyAdventure.h"
#include "MartyCharacter.h"
#include "HalloweenSpecial.h"

void UCozyAdventure::OpenHalloween()
{
    if(!Progress)return;
    HalloweenReturn=Screen;
    SetScreen(ECozyScreen::Halloween);
}
TArray<FString> UCozyAdventure::HalloweenMenu() const
{
    return {Progress->HalloweenRoundComplete?TEXT("Take a Halloween stroll"):TEXT("Start / continue Bork or Treat"),Progress->HalloweenActive?TEXT("Halloween mode: ON (pause event)"):TEXT("Halloween mode: OFF (enable event)"),TEXT("Choose optional costumes"),TEXT("Restart sticker trail (clears stamps)"),TEXT("Back")};
}
void UCozyAdventure::ConfirmHalloween()
{
    auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();if(!M||!M->Festival)return;
    if(Selection==0)M->Festival->StartTrail();
    else if(Selection==1)M->Festival->Toggle();
    else if(Selection==2){WardrobeReturn=ECozyScreen::Halloween;SetScreen(ECozyScreen::Wardrobe);}
    else if(Selection==3)M->Festival->Replay();
    else Back();
}
