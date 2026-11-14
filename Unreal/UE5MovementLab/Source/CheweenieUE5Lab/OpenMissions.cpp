#include "OpenMissions.h"
#include "CozyAdventure.h"
#include "MartyCharacter.h"
#include "Neighbourhood.h"
#include "CatCompanions.h"
#include "HomeProgression.h"
#include "DogLife.h"
#include "CozyNavigation.h"
#include "RainbowGuy.h"
#include "ParkSports.h"
#include "Kismet/GameplayStatics.h"

const TArray<FOpenMission>& CozyOpenMissions()
{
    static const TArray<FOpenMission> Missions={
        {TEXT("explorer"),TEXT("A Nose for the Neighbourhood"),TEXT("Take the long way round. Find the park and school, in either order."),{{TEXT("park"),TEXT("Explore the park"),1},{TEXT("school"),TEXT("Visit the school approach"),1}}},
        {TEXT("neighbours"),TEXT("Hello, Neighbour!"),TEXT("Make friends at your own pace. Say hello to four different neighbourhood dogs."),{{TEXT("dogs"),TEXT("Greet different dogs"),4}}},
        {TEXT("cats"),TEXT("The Cozy Crew"),TEXT("Share a gentle head bump with Lakitu and Rio. Open Cat companions to call them over."),{{TEXT("lakitu"),TEXT("Head bump with Lakitu"),1},{TEXT("rio"),TEXT("Head bump with Rio"),1}}},
        {TEXT("rainbow"),TEXT("Rainbow Guy's Day Out"),TEXT("Find Marty's favourite squeaky plush at home. Carry him, then stow him safely in the backpack."),{{TEXT("rainbow-carry"),TEXT("Carry Rainbow Guy"),1},{TEXT("rainbow-stow"),TEXT("Stow Rainbow Guy in the backpack"),1}}},
        {TEXT("digging"),TEXT("Chief Sniff Inspector"),TEXT("Investigate all three dig patches around the yard and park. One proper dig in each is enough."),{{TEXT("patches"),TEXT("Dig in different patches"),3}}},
        {TEXT("cozy"),TEXT("A Little Cheeween Cozy"),TEXT("Enjoy a kitchen meal, then earn a Rested bonus in Marty's bed. Either order is welcome."),{{TEXT("meal"),TEXT("Enjoy a kitchen meal"),1},{TEXT("rest"),TEXT("Rest in Marty's bed for three seconds"),1}}},
        {TEXT("sports"),TEXT("Park Playdate"),TEXT("Try all three park games. Score a soccer goal, finish a baseball run and sink a basket. No mission timer."),{{TEXT("soccer"),TEXT("Score a soccer goal"),1},{TEXT("baseball"),TEXT("Finish a baseball run"),1},{TEXT("basketball"),TEXT("Score a basket"),1}}},
        {TEXT("patrol"),TEXT("The Great Cozy Patrol"),TEXT("The original home adventure is here whenever you want it: find the golden snacks, visit the cats, then the blanket fort."),{{TEXT("snacks"),TEXT("Find golden snacks"),3},{TEXT("visits"),TEXT("Complete cozy patrol visits"),3}}}
    };return Missions;
}
const FOpenMission* CozyFindMission(FName Id){for(const auto& M:CozyOpenMissions())if(M.Id==Id)return &M;return nullptr;}
int32 UCozyAdventure::MissionValue(FName Key) const{return Progress?FMath::Max(0,Progress->OpenMissionProgress.FindRef(Key)):0;}
int32 UCozyAdventure::MissionPercent(FName Id) const
{
    const auto* M=CozyFindMission(Id);if(!M||!Progress)return 0;if(Progress->FinishedOpenMissions.Contains(Id))return 100;
    int32 Have=0,Total=0;for(const auto& G:M->Goals){Have+=FMath::Min(MissionValue(G.Key),G.Target);Total+=G.Target;}return Total?Have*100/Total:0;
}
int32 UCozyAdventure::FinishedMissionCount() const{int32 N=0;if(Progress)for(const auto& M:CozyOpenMissions())N+=Progress->FinishedOpenMissions.Contains(M.Id)?1:0;return N;}
void UCozyAdventure::RecordMission(FName Key,int32 Value)
{
    if(!Progress||!Enabled||Screen!=ECozyScreen::Playing||Value<=0)return;
    int32 Limit=0;for(const auto& M:CozyOpenMissions())for(const auto& G:M.Goals)if(G.Key==Key)Limit=FMath::Max(Limit,G.Target);
    if(!Limit)return;const int32 Bounded=FMath::Clamp(Value,0,Limit);if(MissionValue(Key)>=Bounded)return;
    Progress->OpenMissionProgress.Add(Key,Bounded);
    for(const auto& M:CozyOpenMissions())if(!Progress->FinishedOpenMissions.Contains(M.Id)&&MissionPercent(M.Id)==100)
    {
        Progress->FinishedOpenMissions.AddUnique(M.Id);
        MissionNotice=TEXT("Mission complete: ")+M.Title+TEXT("  |  Journal pawprint earned");MissionNoticeTime=7;PlayCue(TEXT("Discovery"));
    }
    Save();if(Progress->NavTarget==0)if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Navigation)M->Navigation->RefreshRoute();
}
void UCozyAdventure::TickOpenMissions(float Delta)
{
    MissionNoticeTime=FMath::Max(0.f,MissionNoticeTime-Delta);if(MissionNoticeTime==0)MissionNotice.Empty();
    if(Screen!=ECozyScreen::Playing)return;MissionPollTime-=Delta;if(MissionPollTime>0)return;MissionPollTime=.5f;
    auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();auto* P=Marty();if(!M||!P||!Progress)return;
    if(!Progress->TrackedOpenMission.IsNone()&&!CozyFindMission(Progress->TrackedOpenMission))Progress->TrackedOpenMission=NAME_None;
    if(M->Neighbourhood&&M->Neighbourhood->Outside)
    {
        const FVector Here=P->GetActorLocation();auto Visit=[&](FName Key,FVector At,float Radius){if(FVector::Dist2D(Here,At)<Radius&&FMath::Abs(Here.Z-At.Z)<180)RecordMission(Key);};
        Visit(TEXT("park"),M->Neighbourhood->Park,750);Visit(TEXT("school"),M->Neighbourhood->School,650);
    }
    int32 Dogs=0;for(uint32 Bits=uint32(Progress->NeighbourDogMask);Bits;Bits>>=1)Dogs+=Bits&1;RecordMission(TEXT("dogs"),Dogs);
    if(Progress->LakituBond>0)RecordMission(TEXT("lakitu"));if(Progress->RioBond>0)RecordMission(TEXT("rio"));
    if(Progress->CarryItem==RainbowGuyItem)RecordMission(TEXT("rainbow-carry"));
    if(Progress->BagItems.Contains(RainbowGuyItem)){RecordMission(TEXT("rainbow-carry"));RecordMission(TEXT("rainbow-stow"));}
    int32 Patches=0;for(int32 D:Progress->HoleDepths)Patches+=D>0?1:0;RecordMission(TEXT("patches"),Patches);
    if(Progress->MealBonusRemaining>0)RecordMission(TEXT("meal"));if(Progress->RestBonusRemaining>0)RecordMission(TEXT("rest"));
    int32 Snacks=0;for(uint32 Bits=uint32(Progress->SnackMask);Bits;Bits>>=1)Snacks+=Bits&1;
    // Authored patrols may have different stop counts; normalize only this optional journal entry.
    RecordMission(TEXT("snacks"),Mission&&!Mission->Snacks.IsEmpty()?Snacks*3/Mission->Snacks.Num():0);
    RecordMission(TEXT("visits"),Mission&&!Mission->Visits.IsEmpty()?Progress->VisitIndex*3/Mission->Visits.Num():0);
}
void UCozyAdventure::TrackMission(FName Id)
{
    if(!Progress||(!Id.IsNone()&&!CozyFindMission(Id)))return;
    Progress->TrackedOpenMission=Id;Progress->NavTarget=0;Save();
    if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Navigation)M->Navigation->RefreshRoute();
}
void UCozyAdventure::OpenMissions()
{
    if(!Enabled||!Progress)return;
    if(Screen==ECozyScreen::Missions){Back();return;}
    if(Screen!=ECozyScreen::Playing&&Screen!=ECozyScreen::Pause&&Screen!=ECozyScreen::Title)return;
    MissionReturn=Screen;SetScreen(ECozyScreen::Missions);
    for(int32 I=0;I<CozyOpenMissions().Num();++I)if(CozyOpenMissions()[I].Id==Progress->TrackedOpenMission)Selection=I;
}
FString UCozyAdventure::MissionNext(FName Id) const
{
    const auto* M=CozyFindMission(Id);if(!M)return TEXT("Explore freely. Open Missions whenever you like.");
    if(MissionPercent(Id)==100)return TEXT("Complete! Keep exploring or choose another mission.");
    for(const auto& G:M->Goals)if(MissionValue(G.Key)<G.Target)return FString::Printf(TEXT("%s: %d / %d"),*G.Label,MissionValue(G.Key),G.Target);return FString();
}
FString UCozyAdventure::MissionTracker() const
{
    const auto* M=Progress?CozyFindMission(Progress->TrackedOpenMission):nullptr;
    return M?FString::Printf(TEXT("%s | %d%%"),*M->Title,MissionPercent(M->Id)):TEXT("Free exploring | J / Pause > Missions");
}
TArray<FString> UCozyAdventure::MissionRows() const
{
    TArray<FString> Rows;for(const auto& M:CozyOpenMissions())Rows.Add((MissionPercent(M.Id)==100?FString(TEXT("[Done] ")):Progress->TrackedOpenMission==M.Id?FString(TEXT("[Pinned] ")):FString())+M.Title);
    Rows.Add(TEXT("Unpin mission / explore freely"));Rows.Add(Progress->ShowMissionTracker?TEXT("Mission tracker: ON"):TEXT("Mission tracker: OFF"));Rows.Add(TEXT("Back"));return Rows;
}
bool UCozyAdventure::MissionWaypoint(FVector& Position,bool& Outdoors,FString& Label) const
{
    const auto* Def=Progress?CozyFindMission(Progress->TrackedOpenMission):nullptr;const auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();if(!Def||!M||!M->Neighbourhood||MissionPercent(Def->Id)==100)return false;
    FName Key;for(const auto& G:Def->Goals)if(MissionValue(G.Key)<G.Target){Key=G.Key;break;}Outdoors=false;Label=MissionNext(Def->Id);
    if(Key==TEXT("park")){Position=M->Neighbourhood->Park;Outdoors=true;}
    else if(Key==TEXT("school")){Position=M->Neighbourhood->School;Outdoors=true;}
    else if(Key==TEXT("dogs")){for(int32 I=0;I<M->Neighbourhood->Dogs.Num()&&I<31;++I)if(!(Progress->NeighbourDogMask&(1<<I))){Position=M->Neighbourhood->Dogs[I].Position;Outdoors=true;return true;}return false;}
    else if(Key==TEXT("lakitu")||Key==TEXT("rio")){if(!M->Companions)return false;const int32 I=Key==TEXT("lakitu")?0:1;Position=M->Companions->Position(I);Outdoors=M->Companions->Cats[I].Outdoors;}
    else if(Key==TEXT("meal"))Position=FVector(-180,320,0);
    else if(Key==TEXT("rest")){if(!M->HomeProgression)return false;Position=M->HomeProgression->Bed;}
    else if(Key==TEXT("patches")){const auto* P=Marty();if(!P||!P->Life)return false;int32 I=0;while(Progress->HoleDepths.IsValidIndex(I)&&Progress->HoleDepths[I]>0)++I;if(!P->Life->Patches.IsValidIndex(I))return false;Position=P->Life->Patches[I];}
    else if(Key==TEXT("soccer")||Key==TEXT("baseball")||Key==TEXT("basketball")){if(!M->Neighbourhood->Sports)return false;auto* S=M->Neighbourhood->Sports.Get();Position=Key==TEXT("soccer")?S->Soccer:Key==TEXT("baseball")?S->Base(0):S->Court;Outdoors=true;}
    else if(Key==TEXT("rainbow-stow"))return false;
    else if(Def->Id==TEXT("patrol"))return false; // Existing live patrol waypoint stays authoritative.
    else Position=FVector(70,-310,7);return true;
}
