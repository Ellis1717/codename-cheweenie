#include "CozyAdventure.h"
#include "Kismet/GameplayStatics.h"

void UCozyAdventure::CheckRatchetUnlock()
{
    if(!Progress||!Progress->Complete||Progress->RatchetUnlocked)return;
    Progress->RatchetUnlocked=true;
    Message=(Message.IsEmpty()?FString():Message+TEXT("  "))+TEXT("A cozy place for an old friend. Welcome home, Ratchet! Memory walk unlocked.");
    HoldMessage(12);
    Save();
}
void UCozyAdventure::VisitRatchet()
{
    CheckRatchetUnlock();
    if(!Progress||!Progress->RatchetUnlocked){Message=TEXT("Finish The Great Cozy Patrol to make a cozy place for Ratchet.");HoldMessage(7);return;}
    Save();
    UGameplayStatics::OpenLevel(this,TEXT("/Game/VisualLab/Comparison"),true,TEXT("game=/Script/CheweenieUE5Lab.CozyCoopMode?RatchetVisit=1"));
}
