#include "CozyModeHub.h"
#include "ControllerComfort.h"
#include "CozyAdventure.h"
#include "CozyCoop.h"
#include "MartyCharacter.h"
#include "Neighbourhood.h"
#include "HalloweenSpecial.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"

// Stable names make preferences additive and tolerate missing/unknown fields.
static const TCHAR* OverlayLabels[]={TEXT("All gameplay overlays"),TEXT("Objective"),TEXT("Control hints"),TEXT("Pet status / activity"),TEXT("Health meter"),TEXT("Energy meter"),TEXT("Backpack summary"),TEXT("Interaction prompts"),TEXT("World objective labels"),TEXT("Local map"),TEXT("Compass"),TEXT("Current location"),TEXT("Destination guidance"),TEXT("Halloween paw-pass"),TEXT("Direct trick guide"),TEXT("Messages / notifications"),TEXT("LAN / memory room status"),TEXT("Other pet name labels"),TEXT("Home upgrades / bonuses"),TEXT("Cat reaction labels")};
static_assert(UE_ARRAY_COUNT(OverlayLabels)==int32(ECozyOverlay::Count));
static FString OverlayPrefsPath(){return FPaths::ProjectSavedDir()/(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOverlaySmoke"))?TEXT("OverlayAcceptance.json"):TEXT("OverlayPreferences.json"));}
bool UCozyModeHub::OverlayVisible(ECozyOverlay Id) const
{
    const uint32 Index=uint32(Id);return Index<uint32(ECozyOverlay::Count)&&(HiddenOverlays&((1u<<uint32(ECozyOverlay::All))|(1u<<Index)))==0;
}
void UCozyModeHub::LoadOverlays()
{
    HiddenOverlays=0;HUDStyle=0;FString Json;TSharedPtr<FJsonObject> Data;
    if(!FFileHelper::LoadFileToString(Json,*OverlayPrefsPath())||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Data)||!Data)return;
    double StyleValue=0;if(Data->TryGetNumberField(TEXT("style"),StyleValue)&&FMath::IsFinite(StyleValue)&&StyleValue>=0&&StyleValue<4&&StyleValue==FMath::FloorToDouble(StyleValue))HUDStyle=int32(StyleValue);
    for(int32 I=0;I<int32(ECozyOverlay::Count);++I){bool Visible=true;if(Data->TryGetBoolField(OverlayLabels[I],Visible)&&!Visible)HiddenOverlays|=1u<<I;}
}
void UCozyModeHub::SaveOverlays()
{
    auto Data=MakeShared<FJsonObject>();for(int32 I=0;I<int32(ECozyOverlay::Count);++I)Data->SetBoolField(OverlayLabels[I],(HiddenOverlays&(1u<<I))==0);
    Data->SetNumberField(TEXT("style"),HUDStyle);
    FString Json;FJsonSerializer::Serialize(Data,TJsonWriterFactory<>::Create(&Json));
    if(!FFileHelper::SaveStringToFile(Json,*OverlayPrefsPath()))Status=TEXT("Overlay change applied, but could not save preferences on this PC.");
}
void UCozyModeHub::ToggleOverlay(ECozyOverlay Id){if(uint32(Id)>=uint32(ECozyOverlay::Count))return;HiddenOverlays^=1u<<uint32(Id);SaveOverlays();}
void UCozyModeHub::ResetOverlays(){HiddenOverlays=0;HUDStyle=0;SaveOverlays();}
FString UCozyModeHub::OverlayStyleName() const{static const TCHAR* Names[]={TEXT("Classic Teal"),TEXT("Cozy Paper"),TEXT("Midnight Pop"),TEXT("Clean Slate")};return Names[FMath::Clamp(HUDStyle,0,3)];}
void UCozyModeHub::AdjustOverlayStyle(int32 Direction){if(!Direction)return;HUDStyle=(HUDStyle+(Direction>0?1:3))%4;SaveOverlays();}

static const TCHAR* MapPath=TEXT("/Game/VisualLab/Comparison");
FString CozyHubProfileSlot()
{
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubSmoke"))||FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubArchiveSmoke")))return FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubGuest"))?TEXT("CheweenieHubGuestAcceptance"):TEXT("CheweenieHubAcceptance");
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetSmoke"))||FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetLan")))return TEXT("CheweenieRatchetAcceptance");
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCoopSmoke")))return TEXT("CheweenieCoopAcceptance");
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOverlaySmoke")))return TEXT("CheweenieOverlayAcceptance");
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieSmoke")))return TEXT("CheweenieHubPresentationAcceptance");
    return TEXT("CheweenieAdventureV1");
}
UWorld* UCozyModeHub::GetWorld() const{return GetGameInstance()?GetGameInstance()->GetWorld():nullptr;}
UCozyAdventure* UCozyModeHub::Session() const{const auto* M=GetWorld()?GetWorld()->GetAuthGameMode<AMartyPlayMode>():nullptr;return M?M->Adventure.Get():nullptr;}
static FString NetworkPrefsPath(){return FPaths::ProjectSavedDir()/(FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubSmoke"))?TEXT("ModeHubAcceptance.json"):TEXT("ModeHubPreferences.json"));}
void UCozyModeHub::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    if(GEngine){GEngine->OnNetworkFailure().AddUObject(this,&UCozyModeHub::NetworkFailed);GEngine->OnTravelFailure().AddWeakLambda(this,[this](UWorld* World,ETravelFailure::Type,const FString& Error){NetworkFailed(World,nullptr,ENetworkFailure::FailureReceived,Error);});}
    FString Json;TSharedPtr<FJsonObject> Data;
    if(FFileHelper::LoadFileToString(Json,*NetworkPrefsPath())&&FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Data)&&Data)
    {
        FString Host;if(Data->TryGetStringField(TEXT("address"),Host)){FIPv4Address Parsed;if(FIPv4Address::Parse(Host,Parsed)){Octets[0]=Parsed.A;Octets[1]=Parsed.B;Octets[2]=Parsed.C;Octets[3]=Parsed.D;}}
        double Value;if(Data->TryGetNumberField(TEXT("port"),Value)&&FMath::IsFinite(Value))Port=FMath::Clamp(int32(FMath::Clamp(Value,1024.,65535.)),1024,65535);
    }
    int32 Override=0;if(FParse::Value(FCommandLine::Get(),TEXT("CheweenieHubPort="),Override))Port=FMath::Clamp(Override,1024,65535);
    if((FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubSmoke"))||FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubArchiveSmoke")))&&!FParse::Param(FCommandLine::Get(),TEXT("CheweenieArchiveReturned")))UGameplayStatics::DeleteGameInSlot(CozyHubProfileSlot(),0);
    LoadOverlays();
    LoadArchives();
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieArchiveError")))Status=TEXT("That archived build could not be verified. The latest game is ready.");
}
void UCozyModeHub::Deinitialize(){if(GEngine){GEngine->OnNetworkFailure().RemoveAll(this);GEngine->OnTravelFailure().RemoveAll(this);}Super::Deinitialize();}
void UCozyModeHub::LoadArchives()
{
    FString Path,Json;TSharedPtr<FJsonObject> Data;
    if(!FParse::Value(FCommandLine::Get(),TEXT("CheweenieArchiveIndex="),Path)||!FFileHelper::LoadFileToString(Json,*Path)||!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json),Data)||!Data)return;
    const TArray<TSharedPtr<FJsonValue>>* Entries;
    if(!Data->TryGetArrayField(TEXT("entries"),Entries))return;
    for(const auto& Value:*Entries){if(!Value.IsValid()||Value->Type!=EJson::Object)continue;const auto Entry=Value->AsObject();FString Id,Label;if(Entry&&Entry->TryGetStringField(TEXT("id"),Id)&&Entry->TryGetStringField(TEXT("label"),Label)&&!Id.IsEmpty()&&Id.Len()<100&&Archives.Num()<64)Archives.Add({Id,Label.Left(100).Replace(TEXT("\n"),TEXT(" "))});}
}
FString UCozyModeHub::Address() const{return FString::Printf(TEXT("%d.%d.%d.%d"),Octets[0],Octets[1],Octets[2],Octets[3]);}
void UCozyModeHub::SaveNetworkPreferences(){auto Data=MakeShared<FJsonObject>();Data->SetStringField(TEXT("address"),Address());Data->SetNumberField(TEXT("port"),Port);FString Json;FJsonSerializer::Serialize(Data,TJsonWriterFactory<>::Create(&Json));FFileHelper::SaveStringToFile(Json,*NetworkPrefsPath());}
TArray<FString> UCozyModeHub::HostAddresses() const
{
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubSmoke")))return {TEXT("127.0.0.1")};
    TArray<FString> Result;TArray<TSharedPtr<FInternetAddr>> Addresses;
    if(auto* S=ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM))if(S->GetLocalAdapterAddresses(Addresses))for(const auto& A:Addresses){const FString Value=A->ToString(false);if(Value.Contains(TEXT("."))&&!Value.StartsWith(TEXT("127."))&&!Value.StartsWith(TEXT("169.254.")))Result.AddUnique(Value);}
    if(Result.IsEmpty())Result.Add(TEXT("No Wi-Fi / Ethernet IPv4 address found"));return Result;
}
bool UCozyModeHub::RatchetAvailable() const
{
    if(const auto* S=Session())if(S->Progress)return S->Progress->RatchetUnlocked||S->Progress->Complete;
    const auto* Save=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(CozyHubProfileSlot(),0));return Save&&(Save->RatchetUnlocked||Save->Complete);
}
void UCozyModeHub::NewPage(ECozyHubPage Next){Page=Next;Selection=0;GetGameInstance()->GetSubsystem<UControllerComfort>()->ClearLocalInput();}
void UCozyModeHub::Open(){NewPage(ECozyHubPage::Modes);}
TArray<FString> UCozyModeHub::Rows() const
{
    switch(Page)
    {
    case ECozyHubPage::ControllerSetup:return GetGameInstance()->GetSubsystem<UControllerComfort>()->Rows();
    case ECozyHubPage::Modes:if(Session())return {TEXT("Play / continue adventure"),TEXT("Host Cozy Crew LAN room"),TEXT("Join Cozy Crew LAN room"),TEXT("Graphics, overlays & controls"),FString::Printf(TEXT("Archived versions (%d)"),Archives.Num()),TEXT("Back / resume"),TEXT("Quit game")};else return {TEXT("Resume this room"),TEXT("Return to core adventure"),TEXT("Host Cozy Crew LAN room"),TEXT("Join Cozy Crew LAN room"),TEXT("Graphics, overlays & controls"),FString::Printf(TEXT("Archived versions (%d)"),Archives.Num()),TEXT("Quit game")};
    case ECozyHubPage::Join:return {Connecting?TEXT("Connecting..."):FString(TEXT("Join "))+Address()+FString::Printf(TEXT(":%d"),Port),FString::Printf(TEXT("Address part 1: %d"),Octets[0]),FString::Printf(TEXT("Address part 2: %d"),Octets[1]),FString::Printf(TEXT("Address part 3: %d"),Octets[2]),FString::Printf(TEXT("Address part 4: %d"),Octets[3]),FString::Printf(TEXT("Port: %d"),Port),Connecting?TEXT("Cancel connection"):TEXT("Back")};
    case ECozyHubPage::Host:{TArray<FString> R={TEXT("Start hosting - up to two players"),FString::Printf(TEXT("Port: %d"),Port)};for(const auto& A:HostAddresses())R.Add(TEXT("Share host address: ")+A);R.Add(TEXT("Back"));return R;}
    case ECozyHubPage::Archives:{TArray<FString> R;for(const auto& A:Archives)R.Add(A.Label);if(R.IsEmpty())R.Add(TEXT("Use Play Cheweenie to load local archives"));R.Add(TEXT("Back"));return R;}
    case ECozyHubPage::Graphics:{const TCHAR* Levels[]={TEXT("LOW - smooth"),TEXT("MEDIUM - short hairs"),TEXT("HIGH - more hairs")};return {FString(TEXT("Fur detail: "))+Levels[const_cast<UCozyModeHub*>(this)->FurDetail()],Session()?TEXT("Full controls, cameras & sound"):TEXT("Full solo controls (leave room)"),TEXT("Keyboard & controller guide"),TEXT("Screen overlays"),FString(TEXT("Overlay look: "))+OverlayStyleName(),TEXT("Controller comfort setup"),TEXT("Back")};}
    case ECozyHubPage::Overlays:{TArray<FString> R;for(int32 I=0;I<int32(ECozyOverlay::Count);++I)R.Add(FString(OverlayLabels[I])+((HiddenOverlays&(1u<<I))?TEXT(": OFF"):TEXT(": ON")));R.Add(TEXT("Restore overlay defaults"));R.Add(FString(TEXT("Overlay look: "))+OverlayStyleName());R.Add(TEXT("Back"));return R;}
    case ECozyHubPage::Help:return {TEXT("WASD / left stick: move"),TEXT("mouse / right stick: look | V / R3: camera"),TEXT("Esc / Options: pause / modes"),TEXT("Menus: Arrows / D-pad | Enter / A-Cross"),TEXT("Arrows / D-pad: settings | B-Circle: back"),TEXT("Solo: hold L1/LB + Cross / A: lie | Circle / B: roll"),TEXT("Solo: hold L1/LB + Square / X: shake | Triangle / Y: bark"),TEXT("Hold L1/LB for direct tricks (solo)"),TEXT("LAN: brackets / D-pad left-right: choose pet"),TEXT("Back")};
    case ECozyHubPage::Leave:return {TEXT("Keep playing"),PendingLeave==TEXT("quit")?TEXT("Leave session and quit"):TEXT("Leave session and switch")};
    }return {};
}
FString UCozyModeHub::Heading() const{if(Page==ECozyHubPage::ControllerSetup)return TEXT("Controller comfort");return Page==ECozyHubPage::Overlays?TEXT("Screen overlays"):Page==ECozyHubPage::Modes?TEXT("Codename .... Cheweenie"):Page==ECozyHubPage::Host?TEXT("Host a Cozy Crew room"):Page==ECozyHubPage::Join?TEXT("Join a Cozy Crew room"):Page==ECozyHubPage::Archives?TEXT("Archived versions"):Page==ECozyHubPage::Leave?TEXT("Leave this room?"):Page==ECozyHubPage::Help?TEXT("Keyboard & controller guide"):TEXT("Graphics");}
FString UCozyModeHub::Hint() const
{
    if(!Status.IsEmpty())return Status;
    if(Page==ECozyHubPage::Overlays)return TEXT("Saved on this PC for solo & LAN. Master OFF hides gameplay overlays; menus remain available.");
    if(Page==ECozyHubPage::Join)return TEXT("Use left/right to edit each address part. Both PCs need this same build.");
    if(Page==ECozyHubPage::Host)return TEXT("Share this PC's address and port. LAN room: pets, shared toy and voices.");
    if(Page==ECozyHubPage::Archives)return TEXT("Older builds open separately. Quit the archive to return here; latest progress is retained.");
    if(Page==ECozyHubPage::Leave)return GetWorld()&&GetWorld()->GetNetMode()==NM_ListenServer?TEXT("Leaving ends your hosted room for both players. Solo progress is retained."):TEXT("You leave this room; the host and other player can keep playing.");
    return TEXT("One adventure: home, neighbourhood, activities and unlocks. Older builds stay archived.");
}
void UCozyModeHub::Navigate(int32 D){const int32 N=Rows().Num();if(N)Selection=(Selection+D+N)%N;}
bool UCozyModeHub::CanAdjust(int32 Row) const{return (Page==ECozyHubPage::ControllerSetup&&Row>=0&&Row<6)||(Page==ECozyHubPage::Overlays&&Row>=0&&Row<int32(ECozyOverlay::Count))||(Page==ECozyHubPage::Graphics&&(Row==0||Row==4))||(Page==ECozyHubPage::Overlays&&Row==int32(ECozyOverlay::Count)+1)||(!Connecting&&((Page==ECozyHubPage::Join&&Row>=1&&Row<=5)||(Page==ECozyHubPage::Host&&Row==1)));}
void UCozyModeHub::Adjust(int32 D)
{
    if(!D||Connecting)return;
    if(Page==ECozyHubPage::ControllerSetup){GetGameInstance()->GetSubsystem<UControllerComfort>()->Adjust(Selection,D);return;}
    if((Page==ECozyHubPage::Graphics&&Selection==4)||(Page==ECozyHubPage::Overlays&&Selection==int32(ECozyOverlay::Count)+1)){AdjustOverlayStyle(D);return;}
    if(Page==ECozyHubPage::Overlays&&Selection>=0&&Selection<int32(ECozyOverlay::Count)){ToggleOverlay(ECozyOverlay(Selection));return;}
    if(Page==ECozyHubPage::Graphics&&Selection==0){AdjustFur(D);return;}
    if(Page==ECozyHubPage::Join&&Selection>=1&&Selection<=4){int32& N=Octets[Selection-1];N=(N+(D>0?1:255))%256;SaveNetworkPreferences();return;}
    if((Page==ECozyHubPage::Join&&Selection==5)||(Page==ECozyHubPage::Host&&Selection==1)){Port=FMath::Clamp(Port+(D>0?1:-1),1024,65535);SaveNetworkPreferences();}
}
void UCozyModeHub::Back()
{
    if(Connecting){Connecting=false;GEngine->CancelPending(GetWorld());Status=TEXT("Connection cancelled.");}
    if(Page==ECozyHubPage::Help||Page==ECozyHubPage::Overlays||Page==ECozyHubPage::ControllerSetup){NewPage(ECozyHubPage::Graphics);return;}
    if(Page!=ECozyHubPage::Modes){NewPage(ECozyHubPage::Modes);return;}
    if(auto* S=Session())S->CloseModes();
    else if(auto* PC=Cast<ACozyCoopController>(UGameplayStatics::GetPlayerController(this,0)))PC->SetHubMenu(false);
}
void UCozyModeHub::Confirm()
{
    Status.Empty();
    if(Page==ECozyHubPage::ControllerSetup){if(Selection==8)Back();else GetGameInstance()->GetSubsystem<UControllerComfort>()->Adjust(Selection,1);return;}
    if(Page==ECozyHubPage::Modes){const bool Room=Session()==nullptr;if(Selection==6){Request(TEXT("quit"));return;}if((Room&&Selection==0)||(!Room&&Selection==5)){Back();return;}const int32 Choice=Selection-(Room?1:0);switch(Choice){case 0:Request(TEXT("continue"));break;case 1:NewPage(ECozyHubPage::Host);break;case 2:NewPage(ECozyHubPage::Join);break;case 3:NewPage(ECozyHubPage::Graphics);break;case 4:NewPage(ECozyHubPage::Archives);break;}return;}
    if(Page==ECozyHubPage::Overlays){if(Selection>=0&&Selection<int32(ECozyOverlay::Count))ToggleOverlay(ECozyOverlay(Selection));else if(Selection==int32(ECozyOverlay::Count))ResetOverlays();else if(Selection==int32(ECozyOverlay::Count)+1)AdjustOverlayStyle(1);else Back();return;}
    if(Page==ECozyHubPage::Leave){if(Selection==1){const FString Action=PendingLeave;PendingLeave.Empty();Execute(Action);}else NewPage(ECozyHubPage::Modes);return;}
    if(Page==ECozyHubPage::Graphics){if(Selection==0)AdjustFur(1);else if(Selection==1){if(auto* S=Session())S->OpenControlsFromModes();else Request(TEXT("controls"));}else if(Selection==2)NewPage(ECozyHubPage::Help);else if(Selection==3)NewPage(ECozyHubPage::Overlays);else if(Selection==4)AdjustOverlayStyle(1);else if(Selection==5)NewPage(ECozyHubPage::ControllerSetup);else Back();return;}
    if(Page==ECozyHubPage::Host){if(Selection==0)Request(TEXT("host"));else if(Selection==Rows().Num()-1)Back();return;}
    if(Page==ECozyHubPage::Join){if(Selection==0&&!Connecting)Request(TEXT("join"));else if(Selection==6)Back();return;}
    if(Page==ECozyHubPage::Archives){if(Archives.IsValidIndex(Selection))Request(TEXT("archive:")+Archives[Selection].Id);else Back();return;}
    if(Page==ECozyHubPage::Help)Back();
}
int32 UCozyModeHub::FurDetail()
{
    if(const auto* S=Session())if(S->Progress){CachedFur=S->FurDetail();FurLoaded=true;return CachedFur;}
    if(!FurLoaded){const auto* Save=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(CozyHubProfileSlot(),0));if(Save)CachedFur=(Save->DisabledFeatures&(1<<uint8(ECozyFeature::DetailedCoat)))?0:FMath::Clamp(Save->FurDetailPreference,1,2);FurLoaded=true;}
    return CachedFur;
}
void UCozyModeHub::AdjustFur(int32 D)
{
    if(!D)return;
    const int32 Level=(FurDetail()+(D>0?1:2))%3;
    auto* S=Session();auto* Save=S&&S->Progress?S->Progress.Get():Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(CozyHubProfileSlot(),0));
    if(!Save)Save=NewObject<UCozyAdventureSave>(this);
    const int32 Bit=1<<uint8(ECozyFeature::DetailedCoat);if(Level==0)Save->DisabledFeatures|=Bit;else{Save->DisabledFeatures&=~Bit;Save->FurDetailPreference=Level;}
    CachedFur=Level;FurLoaded=true;if(S&&S->Progress)S->Save();else UGameplayStatics::SaveGameToSlot(Save,CozyHubProfileSlot(),0);
}
void UCozyModeHub::Request(const FString& Mode)
{
    if(GetWorld()&&GetWorld()->GetNetMode()!=NM_Standalone){PendingLeave=Mode;NewPage(ECozyHubPage::Leave);return;}Execute(Mode);
}
void UCozyModeHub::TravelSolo(const FString& Action)
{
    PendingAction=Action;GetGameInstance()->GetEngine()->SetClientTravel(GetWorld(),TEXT("/Game/VisualLab/Comparison?game=/Script/CheweenieUE5Lab.MartyPlayMode?CozyHub=1"),TRAVEL_Absolute);
}
void UCozyModeHub::Execute(const FString& Mode)
{
    if(Mode==TEXT("ratchet")&&!RatchetAvailable()){Status=TEXT("Finish Cozy Patrol to welcome Ratchet home.");return;}
    FurDetail();if(auto* S=Session())S->Save();
    if(Mode==TEXT("quit")){UGameplayStatics::GetPlayerController(this,0)->ConsoleCommand(TEXT("quit"));return;}
    if(Mode.StartsWith(TEXT("archive:")))
    {
        FString Path,Token;if(!FParse::Value(FCommandLine::Get(),TEXT("CheweenieArchiveRequest="),Path)||!FParse::Value(FCommandLine::Get(),TEXT("CheweenieArchiveToken="),Token)){Status=TEXT("Open Play Cheweenie to switch archived builds.");return;}
        const FString Id=Mode.Mid(8);if(!Archives.ContainsByPredicate([&](const FCozyArchiveChoice& A){return A.Id==Id;}))return;
        auto Data=MakeShared<FJsonObject>();Data->SetStringField(TEXT("id"),Id);Data->SetStringField(TEXT("token"),Token);FString Json;FJsonSerializer::Serialize(Data,TJsonWriterFactory<>::Create(&Json));
        if(!FFileHelper::SaveStringToFile(Json,*Path)){Status=TEXT("Could not prepare that archive. The current game is still available.");return;}
        UGameplayStatics::GetPlayerController(this,0)->ConsoleCommand(TEXT("quit"));return;
    }
    if(Mode==TEXT("host")){Hosting=true;FURL URL(nullptr,MapPath,TRAVEL_Absolute);URL.Port=Port;URL.AddOption(TEXT("game=/Script/CheweenieUE5Lab.CozyCoopMode"));URL.AddOption(TEXT("listen"));SaveNetworkPreferences();GetGameInstance()->GetEngine()->SetClientTravel(GetWorld(),*URL.ToString(),TRAVEL_Absolute);return;}
    if(Mode==TEXT("join")){if(Address()==TEXT("0.0.0.0")||Address()==TEXT("255.255.255.255")){Status=TEXT("Enter the host PC's IPv4 address.");return;}Connecting=true;SaveNetworkPreferences();Status=TEXT("Connecting to the room. Back cancels the connection.");UGameplayStatics::GetPlayerController(this,0)->ClientTravel(Address()+FString::Printf(TEXT(":%d"),Port),TRAVEL_Absolute);return;}
    PendingAction=Mode;
    if(!Session())TravelSolo(Mode);
}
void UCozyModeHub::SoloReady(UCozyAdventure* S)
{
    Returning=false;ReturnQueued=false;Hosting=false;Connecting=false;
    if(!PendingAction.IsEmpty())return;
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieModeHub"))||GetWorld()->URL.HasOption(TEXT("CozyHub")))S->OpenModes();
}
void UCozyModeHub::NetworkFailed(UWorld* World,UNetDriver*,ENetworkFailure::Type,const FString&)
{
    if((World&&World!=GetWorld())||Returning||(!Connecting&&!Hosting&&GetWorld()->GetNetMode()==NM_Standalone))return;Status=TEXT("The room connection ended or could not be reached. Your solo progress is safe.");Returning=true;ReturnQueued=true;
}
void UCozyModeHub::RoomReady(){Connecting=false;Hosting=false;Status.Empty();Open();}
void UCozyModeHub::Tick(float Delta)
{
    if(ReturnQueued&&GetWorld()&&GetWorld()->HasBegunPlay()){ReturnQueued=false;TravelSolo();}
    if(!PendingAction.IsEmpty())if(auto* S=Session())if(S->Progress&&GetWorld()->HasBegunPlay())
    {
        const FString Action=PendingAction;PendingAction.Empty();auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
        if(Action==TEXT("hub"))S->OpenModes();
        else if(Action==TEXT("controls")){S->OpenModes();S->OpenControlsFromModes();}
        else if(Action==TEXT("continue")){S->CloseModes();if(S->Screen==ECozyScreen::Title){if(S->Progress->HalloweenActive&&M&&M->Festival)M->Festival->StartTrail();else S->Start(false);}else if(S->Screen==ECozyScreen::Pause)S->Back();}
        else if(Action==TEXT("ratchet"))S->VisitRatchet();
        else if(Action==TEXT("halloween")){if(M&&M->Festival)M->Festival->StartTrail();}
        else {if(S->Progress->HalloweenActive&&M&&M->Festival)M->Festival->Toggle();S->Start(false);if(Action==TEXT("explore")&&M&&M->Neighbourhood)M->Neighbourhood->GoOutside(Cast<AMartyCharacter>(UGameplayStatics::GetPlayerPawn(this,0)));}
    }
    TickOverlayAcceptance(Delta);
    TickAcceptance(Delta);
}
