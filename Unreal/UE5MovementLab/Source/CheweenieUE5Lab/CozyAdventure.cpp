#include "CozyAdventure.h"
#include "CatCompanions.h"
#include "CozyNavigation.h"
FString CozyFeatureLabel(ECozyFeature Id);
#include "Neighbourhood.h"
#include "DogLife.h"
#include "MartyCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Parse.h"
#include "Sound/SoundBase.h"

UCozyMissionDefinition::UCozyMissionDefinition()
{
    auto Stop = [](const TCHAR* Id, const TCHAR* Label, FVector Position, float Radius)
    {
        FCozyStop Result; Result.Id = FName(Id); Result.Label = Label; Result.Position = Position; Result.Radius = Radius; return Result;
    };
    Snacks = {
        Stop(TEXT("rug"), TEXT("Rug-side snack"), FVector(-180,150,12), 42),
        Stop(TEXT("floor"), TEXT("Window-side snack"), FVector(-300,-30,12), 42),
        Stop(TEXT("sofa"), TEXT("Sofa-side snack"), FVector(180,-300,12), 42)
    };
    Visits = {
        Stop(TEXT("lakitu"), TEXT("Lakitu / Keeterman"), FVector(50,0,12), 80),
        Stop(TEXT("rio"), TEXT("Rio / Miss Priss"), FVector(50,170,12), 80),
        Stop(TEXT("cozy"), TEXT("Blanket fort"), FVector(-300,320,12), 85)
    };
    Visits[0].Dialogue=TEXT("Keeterman: Snacks secured. I was supervising the whole time."); Visits[0].SoundName=TEXT("Lakitu");
    Visits[1].Dialogue=TEXT("Rio / Miss Priss: Acceptable service. Your next appointment is blanket time."); Visits[1].SoundName=TEXT("Rio");
    Visits[2].Dialogue=TEXT("Leo Ellis, Blanket Boss: Five more minutes! Marty agrees."); Visits[2].SoundName=TEXT("Cozy");
}

UCozyAdventure::UCozyAdventure() { PrimaryComponentTick.bCanEverTick = true; }

void UCozyAdventure::BeginPlay()
{
    Super::BeginPlay();
    Enabled = !FParse::Param(FCommandLine::Get(), TEXT("CheweenieSmoke"));
    if (!Enabled) { SetComponentTickEnabled(false); return; }
    Slot = FParse::Param(FCommandLine::Get(),TEXT("CheweenieGaitSmoke")) ? TEXT("CheweenieGaitAcceptance") : FParse::Param(FCommandLine::Get(), TEXT("CheweenieAdventureSmoke")) ? TEXT("CheweenieAdventureAcceptance") : TEXT("CheweenieAdventureV1");
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieOutsideSmoke"))){Slot=TEXT("CheweenieOutsideAcceptance");UGameplayStatics::DeleteGameInSlot(Slot,0);}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieLifeSmoke"))){Slot=TEXT("CheweenieLifeAcceptance");if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieLifeResume")))UGameplayStatics::DeleteGameInSlot(Slot,0);}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieAudioSmoke"))){Slot=TEXT("CheweenieAudioAcceptance");UGameplayStatics::DeleteGameInSlot(Slot,0);}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieTricksSmoke"))){Slot=TEXT("CheweenieTricksAcceptance");if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieTricksResume")))UGameplayStatics::DeleteGameInSlot(Slot,0);}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieGaitSmoke"))) UGameplayStatics::DeleteGameInSlot(Slot,0);
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieAdventureSmoke")) && !FParse::Param(FCommandLine::Get(),TEXT("CheweenieResumeCheck")))
        UGameplayStatics::DeleteGameInSlot(Slot,0);
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieNavigationSmoke"))){Slot=TEXT("CheweenieNavigationAcceptance" );if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieNavigationResume")))UGameplayStatics::DeleteGameInSlot(Slot,0);}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCompanionSmoke"))){Slot=TEXT("CheweenieCompanionAcceptance" );if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieCompanionResume")))UGameplayStatics::DeleteGameInSlot(Slot,0);}
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieWorldSmoke"))){Slot=TEXT("CheweenieWorldAcceptance");UGameplayStatics::DeleteGameInSlot(Slot,0);}
    Mission = NewObject<UCozyMissionDefinition>(this);
    FString AssetPath;
    if (!FParse::Param(FCommandLine::Get(),TEXT("CheweenieAdventureSmoke")) && GConfig->GetString(TEXT("Cheweenie.Adventure"), TEXT("MissionAsset"), AssetPath, GGameIni) && !AssetPath.IsEmpty())
        if (auto* Asset = LoadObject<UCozyMissionDefinition>(nullptr, *AssetPath)) Mission = Asset;
    // Keep masks bounded and reject definitions that would create an unwinnable patrol.
    if (Mission->Snacks.IsEmpty() || Mission->Snacks.Num() > 20 || Mission->Visits.IsEmpty()) Mission = NewObject<UCozyMissionDefinition>(this);
    Progress = Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
    if (!Progress || Progress->Version != 1) Progress = NewObject<UCozyAdventureSave>(this);
    if (Progress->MissionId != Mission->MissionId)
    {
        Progress->MissionId = Mission->MissionId; Progress->SnackMask = 0; Progress->VisitIndex = 0; Progress->Complete = false;
    }
    Progress->DisabledFeatures &= (1<<uint8(ECozyFeature::Count))-1;Progress->NavTarget=FMath::Clamp(Progress->NavTarget,0,127);
    Progress->SnackMask &= (1 << Mission->Snacks.Num()) - 1;
    Progress->VisitIndex = FMath::Clamp(Progress->VisitIndex, 0, Mission->Visits.Num());
    Progress->Complete = Progress->VisitIndex == Mission->Visits.Num();
    if (Progress->SnackMask != (1 << Mission->Snacks.Num()) - 1) { Progress->VisitIndex = 0; Progress->Complete = false; }
    Progress->Music = FMath::Clamp(Progress->Music, 0.f, 1.f);
    Progress->Effects = FMath::Clamp(Progress->Effects, 0.f, 1.f);
    for (const FString& Name : {TEXT("Home"), TEXT("Menu"), TEXT("Discovery"), TEXT("Lakitu"), TEXT("Rio"), TEXT("Cozy"), TEXT("Backyard"), TEXT("Step"), TEXT("Jump"), TEXT("Land"), TEXT("Bark"), TEXT("Dig"), TEXT("Crunch"), TEXT("Toy")})
    {
        const FString Path = TEXT("/Game/Adventure/Audio/") + Name + TEXT(".") + Name;
        if (auto* Sound = LoadObject<USoundBase>(nullptr, *Path)) Sounds.Add(Name, Sound);
    }
    BuildFort(); CompleteRoom(); RebuildMarkers(); UpdateMusic(); SetScreen(ECozyScreen::Title);
}

AMartyCharacter* UCozyAdventure::Marty() const { return Cast<AMartyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)); }
void UCozyAdventure::SetScreen(ECozyScreen Next)
{
    Screen = Next; Selection = 0;
    if (auto* Pawn = Marty()) Pawn->GetCharacterMovement()->StopMovementImmediately();
    if (auto* PC=UGameplayStatics::GetPlayerController(this,0))
    {
        PC->bShowMouseCursor=Next!=ECozyScreen::Playing;
        PC->bEnableClickEvents=Next!=ECozyScreen::Playing;
        if(Next==ECozyScreen::Playing) PC->SetInputMode(FInputModeGameOnly());
        else PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false).SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock));
    }
}
void UCozyAdventure::Pause()
{
    if (!Enabled) return;
    if (Screen == ECozyScreen::Playing) SetScreen(ECozyScreen::Pause);
    else Back();
    PlayCue(TEXT("Menu"));
}
void UCozyAdventure::Navigate(int32 Direction)
{
    if (!Enabled || Screen == ECozyScreen::Playing) return;
    const int32 Count = Menu().Num();
    if (Count) Selection = (Selection + Direction + Count) % Count;
    PlayCue(TEXT("Menu"));
}
void UCozyAdventure::Adjust(int32 Direction)
{
    if(Screen==ECozyScreen::Features){Confirm();return;}
    if (!Enabled || Screen != ECozyScreen::Settings) return;
    if (Selection == 0) Progress->Music = FMath::Clamp(Progress->Music + Direction * .1f, 0.f, 1.f);
    if (Selection == 1) Progress->Effects = FMath::Clamp(Progress->Effects + Direction * .1f, 0.f, 1.f);
    UpdateMusic(); Save(); PlayCue(TEXT("Menu"));
}
void UCozyAdventure::Back()
{
    if (!Enabled) return;
    if(Screen==ECozyScreen::Features){SetScreen(ECozyScreen::Controls);return;}
    if (Screen == ECozyScreen::Controls || Screen == ECozyScreen::Settings || Screen == ECozyScreen::Restart) SetScreen(ReturnScreen);
    else if (Screen == ECozyScreen::Pause || Screen == ECozyScreen::Complete || Screen==ECozyScreen::Backpack || Screen==ECozyScreen::Tricks || Screen==ECozyScreen::Map || Screen==ECozyScreen::Companions) SetScreen(ECozyScreen::Playing);
    else if(Screen==ECozyScreen::Playing)OpenTricks();
}
void UCozyAdventure::OpenBackpack(){if(Screen==ECozyScreen::Backpack){SetScreen(ECozyScreen::Playing);return;}if(Feature(ECozyFeature::Backpack)&&Screen==ECozyScreen::Playing&&Marty()&&Marty()->Life&&!Marty()->Life->Busy())SetScreen(ECozyScreen::Backpack);}
void UCozyAdventure::OpenTricks(){if(Screen==ECozyScreen::Playing&&Marty()&&Marty()->Life&&!Marty()->Life->Busy())SetScreen(ECozyScreen::Tricks);}
void UCozyAdventure::Confirm()
{
    if (!Enabled || !Progress) return;
    UpdateMusic();
    if (Screen == ECozyScreen::Playing) return;
    const ECozyScreen Current = Screen;
    if(Current==ECozyScreen::Companions){const int32 Choice=Selection;SetScreen(ECozyScreen::Playing);if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Companions){auto* C=M->Companions.Get();if(Choice<2)C->ToggleFollow(Choice);else if(Choice==2)C->CallBoth();else if(Choice==3)C->SendHome();else if(Choice==4)C->Pet(0);else if(Choice==5)C->Pet(1);}return;}
    if(Current==ECozyScreen::Map){const auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();const int32 Count=M&&M->Navigation?M->Navigation->Landmarks().Num():0;if(Selection<Count){Progress->NavTarget=Selection;Save();if(M&&M->Navigation)M->Navigation->RefreshRoute();}SetScreen(ECozyScreen::Playing);return;}
    if(Current==ECozyScreen::Controls){if(Selection==0)Back();else if(Selection<=3){FeaturePage=Selection-1;SetScreen(ECozyScreen::Features);}else {const int32 Keep=Selection;SetScreen(ECozyScreen::Playing);if(Marty()&&Marty()->Life)Marty()->Life->ToggleBodyControls();SetScreen(ECozyScreen::Controls);Selection=Keep;}return;}
    if(Current==ECozyScreen::Features){const auto Group=FeatureGroup();if(Group.IsValidIndex(Selection))ToggleFeature(Group[Selection]);else Back();return;}
    if(Current==ECozyScreen::Backpack||Current==ECozyScreen::Tricks)
    {
        const int32 Choice=Selection;SetScreen(ECozyScreen::Playing);auto* L=Marty()?Marty()->Life.Get():nullptr;if(!L)return;
        if(Current==ECozyScreen::Backpack){if(Choice==0)L->Stow();else if(Choice==1)L->Retrieve();}
        else {if(Choice==0)L->LieDown();else if(Choice==1)L->RollOver();else if(Choice==2)L->ShakePaw();else if(Choice==3)L->Bark();else if(Choice==4)L->CyclePace();else if(Choice==5)L->ToggleBodyControls();}return;
    }
    if (Current == ECozyScreen::Title)
    {
        if (Selection == 0) Start(false);
        else if (Selection == 1) { ReturnScreen = Current; SetScreen(ECozyScreen::Restart); }
        else if (Selection == 2) { ReturnScreen = Current; SetScreen(ECozyScreen::Controls); }
        else if (Selection == 3) { ReturnScreen = Current; SetScreen(ECozyScreen::Settings); }
        else if (Selection==4)
        {
            Start(false);
            if(auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(Mode->Neighbourhood&&Marty())Mode->Neighbourhood->GoOutside(Marty());
        }
        else if (auto* PC = UGameplayStatics::GetPlayerController(this,0)) PC->ConsoleCommand(TEXT("quit"));
    }
    else if (Current == ECozyScreen::Pause)
    {
        if (Selection == 0) SetScreen(ECozyScreen::Playing);
        else if (Selection == 1) { if(auto* Pawn=Marty()) Pawn->ReturnHome(); SetScreen(ECozyScreen::Playing); }
        else if (Selection == 2) { ReturnScreen = Current; SetScreen(ECozyScreen::Controls); }
        else if (Selection == 3) { ReturnScreen = Current; SetScreen(ECozyScreen::Settings); }
        else if(Selection==5){SetScreen(ECozyScreen::Playing);OpenCompanions();}else SetScreen(ECozyScreen::Title);
    }
    else if (Current == ECozyScreen::Restart)
    {
        if (Selection == 1) Start(true); else SetScreen(ReturnScreen);
    }
    else if (Current == ECozyScreen::Complete)
    {
        if (Selection == 0) SetScreen(ECozyScreen::Playing);
        else { ReturnScreen = Current; SetScreen(ECozyScreen::Restart); }
    }
    else if(Current == ECozyScreen::Settings) { if(Selection==2) Back(); }
    else Back();
    PlayCue(TEXT("Menu"));
}

void UCozyAdventure::Start(bool Fresh)
{
    if (!Progress) return;
    if (Fresh) { Progress->SnackMask = 0; Progress->VisitIndex = 0; Progress->Complete = false; }
    SetScreen(ECozyScreen::Playing);
    if (auto* Pawn = Marty()) Pawn->ReturnHome();
    RebuildMarkers(); Save(); UpdateMusic();
    Message = TEXT("Fox Ellis, Chief Sniff Inspector: Yep. Definitely snack territory."); MessageTime = 7;
}
void UCozyAdventure::Save()
{
    if (Progress && !Slot.IsEmpty())
        if (!UGameplayStatics::SaveGameToSlot(Progress, Slot, 0)) { Message = TEXT("Could not save progress. You can keep playing."); MessageTime = 6; }
}
FString UCozyAdventure::Objective() const
{
    if (!Progress || !Mission) return TEXT("Getting the patrol ready...");
    if (Progress->Complete) return TEXT("Patrol complete! Explore, visit the crew, or restart from the menu.");
    int32 Count = 0; for(int32 I=0;I<Mission->Snacks.Num();++I) if(Progress->SnackMask & (1<<I)) ++Count;
    if (Count < Mission->Snacks.Num()) return FString::Printf(TEXT("Sniff out the golden snacks: %d / %d"), Count, Mission->Snacks.Num());
    return TEXT("Visit ") + Mission->Visits[Progress->VisitIndex].Label + TEXT(" and press E / X-Square.");
}
FString UCozyAdventure::Prompt() const
{
    const auto* Pawn = Marty();
    if (!Pawn || !Progress || Progress->Complete || Progress->SnackMask != (1<<Mission->Snacks.Num())-1) return FString();
    const auto& Stop = Mission->Visits[Progress->VisitIndex];
    float Radius=Stop.Radius;if(Feature(ECozyFeature::Companions))if(const auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Companions&&M->Companions->CatForStop(Stop.Id)>=0)Radius=FMath::Max(Radius,150.f);
    return FVector::Dist2D(Pawn->GetActorLocation(), StopPosition(Stop)) <= Radius ? TEXT("E / X-Square: ") + Stop.Label : FString();
}
void UCozyAdventure::Interact()
{
    if (!Enabled || Screen != ECozyScreen::Playing) return;
    if(Marty()&&Marty()->Life&&Marty()->Life->Busy())return;
    if(Marty()&&Marty()->Life&&Marty()->Life->Activity==EDogActivity::Lying){Marty()->Life->StandUp();return;}
    if(Prompt().IsEmpty()&&Marty()&&Marty()->Life&&Marty()->Life->Interact())return;
    if(auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(Mode->Neighbourhood&&Marty()&&Mode->Neighbourhood->Interact(Marty()))return;
    if(Prompt().IsEmpty()){if(auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Companions)M->Companions->PetNearby();return;}
    const FCozyStop& Stop = Mission->Visits[Progress->VisitIndex];
    ++Progress->VisitIndex;
    Message = Stop.Dialogue.IsEmpty() ? TEXT("A cozy visit with ") + Stop.Label : Stop.Dialogue;
    PlayCue(Stop.SoundName);
    MessageTime = 7;
    if (Progress->VisitIndex == Mission->Visits.Num())
    {
        Progress->Complete = true; SetScreen(ECozyScreen::Complete);
        MessageTime = 10;
    }
    Save(); RebuildMarkers();
}
void UCozyAdventure::TickComponent(float Delta, ELevelTick TickType, FActorComponentTickFunction* ThisTick)
{
    Super::TickComponent(Delta, TickType, ThisTick);
    if (!Enabled || !Progress) return;
    MessageTime = FMath::Max(0.f, MessageTime - Delta); if(MessageTime == 0) Message.Empty();
    if (Screen != ECozyScreen::Playing || Progress->Complete) return;
    const auto* Pawn = Marty(); if (!Pawn) return;
    for(int32 I=0;I<Mission->Snacks.Num();++I)
        if (!(Progress->SnackMask & (1<<I)) && FVector::Dist2D(Pawn->GetActorLocation(),Mission->Snacks[I].Position) < Mission->Snacks[I].Radius)
        {
            Progress->SnackMask |= 1<<I; PlayCue(TEXT("Discovery")); Save(); RebuildMarkers(); break;
        }
}
TArray<FString> UCozyAdventure::Menu() const
{
    switch(Screen)
    {
    case ECozyScreen::Companions:{const auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();if(!M||!M->Companions)return {TEXT("Close")};const auto* C=M->Companions.Get();return {TEXT("Lakitu: ")+C->State(0)+(C->Following(0)?TEXT(" - ask to stay"):TEXT(" - follow Marty")),TEXT("Rio: ")+C->State(1)+(C->Following(1)?TEXT(" - ask to stay"):TEXT(" - follow Marty")),TEXT("Call both cats here"),TEXT("Send both cats home"),TEXT("Head bump with Lakitu"),TEXT("Head bump with Rio"),TEXT("Close")};}
    case ECozyScreen::Controls:return {TEXT("Back"),TEXT("Navigation & map switches"),TEXT("Pet feature switches"),TEXT("Camera, display & sound switches"),Progress->BodyControls?TEXT("Controls: body sway (switch to standard)"):TEXT("Controls: standard (try body sway)")};
    case ECozyScreen::Features:{TArray<FString> Rows;for(auto Id:FeatureGroup())Rows.Add(CozyFeatureLabel(Id)+(Feature(Id)?TEXT(": ON"):TEXT(": OFF")));Rows.Add(TEXT("Back to controls"));return Rows;}
    case ECozyScreen::Map:{TArray<FString> Rows;if(const auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(M->Navigation){for(const auto& L:M->Navigation->Landmarks())Rows.Add(FString::Printf(TEXT("%d. %s"),Rows.Num()+1,*L.Label));if(Rows.IsValidIndex(Progress->NavTarget))Rows[Progress->NavTarget]=TEXT("[Tracking] ")+Rows[Progress->NavTarget];}Rows.Add(TEXT("Close map"));return Rows;}
    case ECozyScreen::Title: return {TEXT("Continue / start patrol"),TEXT("New patrol"),TEXT("Controls & cameras"),TEXT("Sound settings"),TEXT("Explore the neighbourhood"),TEXT("Quit")};
    case ECozyScreen::Pause: return {TEXT("Resume patrol"),TEXT("Return Marty home"),TEXT("Controls & cameras"),TEXT("Sound settings"),TEXT("Title screen"),TEXT("Cat companions")};
    case ECozyScreen::Backpack: return {TEXT("Stow the toy in Marty's mouth"),TEXT("Take a toy from the backpack"),TEXT("Close backpack")};
    case ECozyScreen::Tricks: return {TEXT("Lie down / get up"),TEXT("Roll over"),TEXT("Shake a paw"),TEXT("Bork!"),TEXT("Change pace: walk / jog / run"),Progress->BodyControls?TEXT("Controls: body sway (switch to standard)"):TEXT("Controls: standard (try two-stick body sway)"),TEXT("Back to exploring")};
    case ECozyScreen::Settings: return {FString::Printf(TEXT("Music: %d%%"),FMath::RoundToInt(Progress->Music*100)),FString::Printf(TEXT("Effects: %d%%"),FMath::RoundToInt(Progress->Effects*100)),TEXT("Back")};
    case ECozyScreen::Restart: return {TEXT("Keep current patrol"),TEXT("Start fresh patrol")};
    case ECozyScreen::Complete: return {TEXT("Keep exploring"),TEXT("Play another patrol")};
    default: return {TEXT("Back")};
    }
}
FString UCozyAdventure::Heading() const
{
    switch(Screen)
    {
    case ECozyScreen::Title: return TEXT("Codename .... Cheweenie");
    case ECozyScreen::Pause: return TEXT("Take a cozy break");
    case ECozyScreen::Backpack: return TEXT("Marty's treasure backpack");
    case ECozyScreen::Tricks: return TEXT("Marty's tricks and pace");
    case ECozyScreen::Companions:return TEXT("Lakitu & Rio - the cozy crew" );
    case ECozyScreen::Controls: return TEXT("Controls & feature switches");
    case ECozyScreen::Features:return FeaturePage==0?TEXT("Navigation switches"):FeaturePage==1?TEXT("Pet feature switches"):TEXT("Camera, display & sound");
    case ECozyScreen::Map:return TEXT("Where shall Marty go?");
    case ECozyScreen::Settings: return TEXT("Sound settings");
    case ECozyScreen::Restart: return TEXT("Start over? Current patrol progress will be replaced.");
    case ECozyScreen::Complete: return TEXT("Cheeween Cozy! Patrol complete.");
    default: return Mission ? Mission->Title : TEXT("Cozy Patrol");
    }
}
void UCozyAdventure::RebuildMarkers()
{
    for(auto& Marker:Markers) if(Marker) Marker->Destroy(); Markers.Empty();
    auto* Mesh=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    auto* Mat=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/VisualLab/UE5Materials/OchreCushion.OchreCushion"));
    for(int32 I=0;I<Mission->Snacks.Num();++I) if(!(Progress->SnackMask & (1<<I)))
    {
        auto* Marker=GetWorld()->SpawnActor<AStaticMeshActor>(Mission->Snacks[I].Position,FRotator::ZeroRotator);
        Marker->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        Marker->GetStaticMeshComponent()->SetStaticMesh(Mesh); Marker->GetStaticMeshComponent()->SetMaterial(0,Mat);
        Marker->SetActorScale3D(FVector(.19,.19,.08)); Marker->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); Markers.Add(Marker);
    }
}
void UCozyAdventure::BuildFort()
{
    FVector Center=Mission->Visits.Last().Position; Center.Z=1;
    if(!FParse::Param(FCommandLine::Get(),TEXT("CheweenieLegacyWorld")))
        if(auto* Blanket=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/Adventure/Scenery/Meshes/FortBlanket.FortBlanket")))
        {auto* Fort=GetWorld()->SpawnActor<AStaticMeshActor>(Center,FRotator::ZeroRotator);Fort->Tags.Add(TEXT("BlanketFortSurface"));Fort->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);Fort->GetStaticMeshComponent()->SetStaticMesh(Blanket);Fort->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Fort->SetActorScale3D(FVector(1,-1,1));return;}
    auto* Mesh=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto* Mat=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/VisualLab/UE5Materials/WovenBlue.WovenBlue"));
    for(int32 I=0;I<3;++I)
    {
        const FVector Offset= I==0 ? FVector(0,0,60) : FVector(0,I==1?-62:62,30);
        auto* Part=GetWorld()->SpawnActor<AStaticMeshActor>(Center+Offset,FRotator::ZeroRotator);
        Part->Tags.Add(TEXT("BlanketFortSurface"));
        Part->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        Part->GetStaticMeshComponent()->SetStaticMesh(Mesh); Part->GetStaticMeshComponent()->SetMaterial(0,Mat);
        Part->SetActorScale3D(I==0 ? FVector(1.7,1.5,.04) : FVector(1.7,.035,.6));
        Part->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
void UCozyAdventure::CompleteRoom()
{
    auto* Mesh=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto* Mat=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/VisualLab/UE5Materials/Limewash.Limewash"));
    const TArray<FVector> Positions={FVector(-490,0,180),FVector(0,-490,180),FVector(-320,490,180)};
    const TArray<FVector> Scales={FVector(.2,10,3.6),FVector(10,.2,3.6),FVector(3.4,.2,3.6)};
    for(int32 I=0;I<Positions.Num();++I)
    {
        auto* Wall=GetWorld()->SpawnActor<AStaticMeshActor>(Positions[I],FRotator::ZeroRotator);
        Wall->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        Wall->GetStaticMeshComponent()->SetStaticMesh(Mesh); Wall->GetStaticMeshComponent()->SetMaterial(0,Mat);
        Wall->SetActorScale3D(Scales[I]); Wall->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
void UCozyAdventure::PlayCue(const FString& Name)
{
    if(Name.IsEmpty()||!Progress||Progress->Effects<=0||!Feature(ECozyFeature::Effects)) return;
    if(!Sounds.Contains(Name))
        if(auto* Asset=LoadObject<USoundBase>(nullptr,*(TEXT("/Game/Adventure/Audio/")+Name+TEXT(".")+Name))) Sounds.Add(Name,Asset);
    if (USoundBase* Sound=Sounds.FindRef(Name)) if(UGameplayStatics::SpawnSound2D(this,Sound,Progress->Effects)){LastCue=Name;++AudibleCueCount;}
}
void UCozyAdventure::UpdateMusic()
{
    if(!Progress)return;
    const auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    const FString Desired=Screen==ECozyScreen::Playing&&Mode&&Mode->Neighbourhood&&Mode->Neighbourhood->Outside?TEXT("Backyard"):TEXT("Home");
    if(!MusicPlayer||MusicTrack!=Desired)
    {
        MusicTrack=Desired;
        if(USoundBase* Sound=Sounds.FindRef(Desired))
        {
            if(MusicPlayer){MusicPlayer->Stop();MusicPlayer->SetSound(Sound);MusicPlayer->Play();}
            else MusicPlayer=UGameplayStatics::SpawnSound2D(this,Sound,FMath::Max(.001f,Progress->Music),1,0,nullptr,false,false);
            if(MusicPlayer)MusicPlayer->FadeIn(.35f,Progress->Music);
        }
    }
    if(MusicPlayer) MusicPlayer->SetVolumeMultiplier(Feature(ECozyFeature::Music)?Progress->Music:0);
    if(MusicPlayer&&Progress->Music>0&&!MusicPlayer->IsPlaying())MusicPlayer->Play();
}
bool UCozyAdventure::MusicPlaying() const{return MusicPlayer&&MusicPlayer->IsPlaying();}
