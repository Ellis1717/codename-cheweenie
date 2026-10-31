#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "MartyCharacter.h"
#include "DogLife.h"

bool UCozyAdventure::Feature(ECozyFeature Id) const{return !Progress||(Progress->DisabledFeatures&(1<<uint8(Id)))==0;}
TArray<ECozyFeature> UCozyAdventure::FeatureGroup() const
{
    TArray<ECozyFeature> Result;const int32 First=FeaturePage==0?0:FeaturePage==1?6:17,Last=FeaturePage==0?6:FeaturePage==1?17:24;
    for(int32 I=First;I<Last;++I)Result.Add(ECozyFeature(I));return Result;
}
void UCozyAdventure::ToggleFeature(ECozyFeature Id)
{
    if(!Progress||Id>=ECozyFeature::Count)return;Progress->DisabledFeatures^=1<<uint8(Id);
    if(auto* P=Marty())if(P->Life&&((Id>=ECozyFeature::Energy&&Id<=ECozyFeature::Hurry)||Id==ECozyFeature::ActionViews))P->Life->CancelAction();
    if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Navigation)M->Navigation->RefreshRoute();
    UpdateMusic();Save();
}
void UCozyAdventure::OpenMap(){if(Screen==ECozyScreen::Map){SetScreen(ECozyScreen::Playing);return;}if(Screen==ECozyScreen::Playing)SetScreen(ECozyScreen::Map);}

// Saved feature IDs are append-only. Display labels can be changed independently.
FString CozyFeatureLabel(ECozyFeature Id)
{
    static const TCHAR* Labels[]={TEXT("Map display"),TEXT("Compass / player heading"),TEXT("Destination guidance"),TEXT("Street route guide"),TEXT("World objective labels"),TEXT("Current location"),TEXT("Energy / tiredness"),TEXT("Health / damage"),TEXT("Backpack storage"),TEXT("Dog tricks"),TEXT("Jump / sofa hop"),TEXT("Dig / bury / recover"),TEXT("Pick up / carry toys"),TEXT("Kitchen meals"),TEXT("Watch TV"),TEXT("Barking"),TEXT("Jog / run / hurry"),TEXT("Mouse / stick camera orbit"),TEXT("Camera presets"),TEXT("Action camera views"),TEXT("Control hints"),TEXT("Detailed Marty coat"),TEXT("Music"),TEXT("Sound effects")};
    return Labels[uint8(Id)];
}
