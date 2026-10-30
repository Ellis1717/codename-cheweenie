#include "CozyAdventure.h"
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
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieGaitSmoke"))) UGameplayStatics::DeleteGameInSlot(Slot,0);
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieAdventureSmoke")) && !FParse::Param(FCommandLine::Get(),TEXT("CheweenieResumeCheck")))
        UGameplayStatics::DeleteGameInSlot(Slot,0);
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
    Progress->SnackMask &= (1 << Mission->Snacks.Num()) - 1;
    Progress->VisitIndex = FMath::Clamp(Progress->VisitIndex, 0, Mission->Visits.Num());
    Progress->Complete = Progress->VisitIndex == Mission->Visits.Num();
    if (Progress->SnackMask != (1 << Mission->Snacks.Num()) - 1) { Progress->VisitIndex = 0; Progress->Complete = false; }
    Progress->Music = FMath::Clamp(Progress->Music, 0.f, 1.f);
    Progress->Effects = FMath::Clamp(Progress->Effects, 0.f, 1.f);
    for (const FString& Name : {TEXT("Home"), TEXT("Menu"), TEXT("Discovery"), TEXT("Lakitu"), TEXT("Rio"), TEXT("Cozy")})
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
    if (!Enabled || Screen != ECozyScreen::Settings) return;
    if (Selection == 0) Progress->Music = FMath::Clamp(Progress->Music + Direction * .1f, 0.f, 1.f);
    if (Selection == 1) Progress->Effects = FMath::Clamp(Progress->Effects + Direction * .1f, 0.f, 1.f);
    UpdateMusic(); Save(); PlayCue(TEXT("Menu"));
}
void UCozyAdventure::Back()
{
    if (!Enabled) return;
    if (Screen == ECozyScreen::Controls || Screen == ECozyScreen::Settings || Screen == ECozyScreen::Restart) SetScreen(ReturnScreen);
    else if (Screen == ECozyScreen::Pause || Screen == ECozyScreen::Complete) SetScreen(ECozyScreen::Playing);
}
void UCozyAdventure::Confirm()
{
    if (!Enabled || !Progress) return;
    if (Screen == ECozyScreen::Playing) return;
    const ECozyScreen Current = Screen;
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
        else SetScreen(ECozyScreen::Title);
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
    return FVector::Dist2D(Pawn->GetActorLocation(), Stop.Position) <= Stop.Radius ? TEXT("E / X-Square: ") + Stop.Label : FString();
}
void UCozyAdventure::Interact()
{
    if (!Enabled || Screen != ECozyScreen::Playing) return;
    if(Marty()&&Marty()->Life&&Marty()->Life->Busy())return;
    if(Prompt().IsEmpty()&&Marty()&&Marty()->Life&&Marty()->Life->Interact())return;
    if(auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>())if(Mode->Neighbourhood&&Marty()&&Mode->Neighbourhood->Interact(Marty()))return;
    if(Prompt().IsEmpty()) return;
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
    case ECozyScreen::Title: return {TEXT("Continue / start patrol"),TEXT("New patrol"),TEXT("Controls & cameras"),TEXT("Sound settings"),TEXT("Explore the neighbourhood"),TEXT("Quit")};
    case ECozyScreen::Pause: return {TEXT("Resume patrol"),TEXT("Return Marty home"),TEXT("Controls & cameras"),TEXT("Sound settings"),TEXT("Title screen")};
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
    case ECozyScreen::Controls: return TEXT("Controls & cameras");
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
    auto* Mesh=LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto* Mat=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/VisualLab/UE5Materials/WovenBlue.WovenBlue"));
    for(int32 I=0;I<3;++I)
    {
        const FVector Offset= I==0 ? FVector(0,0,60) : FVector(0,I==1?-62:62,30);
        auto* Part=GetWorld()->SpawnActor<AStaticMeshActor>(Center+Offset,FRotator::ZeroRotator);
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
    if(Name.IsEmpty()) return;
    if(!Sounds.Contains(Name))
        if(auto* Asset=LoadObject<USoundBase>(nullptr,*(TEXT("/Game/Adventure/Audio/")+Name+TEXT(".")+Name))) Sounds.Add(Name,Asset);
    if (USoundBase* Sound=Sounds.FindRef(Name)) UGameplayStatics::PlaySound2D(this,Sound,Progress->Effects);
}
void UCozyAdventure::UpdateMusic()
{
    if (!MusicPlayer)
        if(USoundBase* Sound=Sounds.FindRef(TEXT("Home"))) MusicPlayer=UGameplayStatics::SpawnSound2D(this,Sound,Progress->Music,1,0,nullptr,false,false);
    if(MusicPlayer) MusicPlayer->SetVolumeMultiplier(Progress->Music);
}
