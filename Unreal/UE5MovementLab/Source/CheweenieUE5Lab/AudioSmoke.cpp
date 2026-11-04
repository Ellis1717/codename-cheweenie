#include "MartyCharacter.h"
#include "NaturalPetAudio.h"
#include "RainbowGuy.h"
#include "DogLife.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "AudioMixerBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "InputKeyEventArgs.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"

// Opt-in acceptance records this game's master submix, not other Windows apps.
void AMartyPlayMode::TickAudioSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    if(!P||!P->Life||!Adventure||!Adventure->Progress)return;auto* S=Adventure.Get();auto* L=P->Life.Get();auto* D=S->Progress.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[PC](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Next=[this](){++SmokeStage;SmokeTime=0;};
    auto Record=[this](){UAudioMixerBlueprintLibrary::StartRecordingOutput(this,8);};
    auto Stop=[&](const TCHAR* Name){UAudioMixerBlueprintLibrary::StopRecordingOutput(this,EAudioRecordingExportType::WavFile,Name,Root);};
    static int32 FirstBarkTake=-1;
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime>5){SmokeCheck(S->LoadedSoundCount()==14,TEXT("All fourteen music/effect assets load from the real map"));SmokeCheck(NaturalCatVoice(TEXT("Lakitu"))&&NaturalCatVoice(TEXT("Rio")),TEXT("Both recorded cat voices load"));SmokeCheck(S->NaturalBarkCount()==4,TEXT("Four recorded small-dog bark takes load"));Key(EKeys::Enter,true);Next();}break;
    case 1:
        if(SmokeTime>.6){Key(EKeys::Enter,false);SmokeCheck(S->MusicTrack==TEXT("Home")&&S->MusicPlaying(),TEXT("Home music plays on the actual audio device"));D->Music=0;D->Effects=.6f;S->UpdateMusic();Next();}break;
    case 2:
        if(SmokeTime>.5){Record();Key(EKeys::B,true);Next();}break;
    case 3:
        if(SmokeTime>.1){Key(EKeys::B,false);SmokeCheck(L->BarkCount==1&&S->LastCue==TEXT("Bark"),TEXT("Keyboard B triggers the bark cue"));FirstBarkTake=S->LastBarkTake;SmokeCheck(FirstBarkTake>=0,TEXT("Keyboard bark uses a recorded take"));Key(EKeys::B,true);Next();}break;
    case 4:
        if(SmokeTime>.1){Key(EKeys::B,false);SmokeCheck(L->BarkCount==1,TEXT("Bark cooldown prevents button spam"));Next();}break;
    case 5:
        if(SmokeTime>1.4){Key(EKeys::Gamepad_LeftShoulder,true);Next();}break;
    case 6:
        if(SmokeTime>.1){Key(EKeys::Gamepad_LeftShoulder,false);Next();}break;
    case 7:
        if(SmokeTime>1){SmokeCheck(L->BarkCount==2&&S->LastCue==TEXT("Bark"),TEXT("Controller LB tap-release triggers the same bark cue"));SmokeCheck(S->LastBarkTake>=0&&S->LastBarkTake!=FirstBarkTake,TEXT("Consecutive keyboard/controller barks select different recordings"));Stop(TEXT("Barks"));D->Music=.3f;D->Effects=0;Neighbourhood->GoOutside(P);S->UpdateMusic();Next();}break;
    case 8:
        if(SmokeTime>.6){SmokeCheck(S->MusicTrack==TEXT("Backyard")&&S->MusicPlaying(),TEXT("Outdoor travel selects looping backyard music"));Record();Next();}break;
    case 9:
        if(SmokeTime>2){Stop(TEXT("Music"));D->Music=0;D->Effects=0;S->UpdateMusic();SmokeYaw=S->AudibleCueCount;Key(EKeys::B,true);Next();}break;
    case 10:
        if(SmokeTime>.6){Key(EKeys::B,false);SmokeCheck(S->AudibleCueCount==SmokeYaw,TEXT("Effects volume zero suppresses bark playback"));Record();Next();}break;
    case 11:
        if(SmokeTime>1){Stop(TEXT("Muted"));P->ReturnHome();S->UpdateMusic();SmokeCheck(S->MusicTrack==TEXT("Home"),TEXT("Return home restores the home music track"));D->Effects=.6f;D->Music=.25f;S->UpdateMusic();SmokeYaw=S->AudibleCueCount;for(const FString& Cue:{TEXT("Dig"),TEXT("Crunch"),TEXT("Toy"),TEXT("Step"),TEXT("Jump"),TEXT("Land"),TEXT("Lakitu"),TEXT("Rio")})S->ActionCue(Cue);SmokeCheck(S->AudibleCueCount==SmokeYaw+8,TEXT("All pet action and companion cues spawn on the audio device"));S->Screen=ECozyScreen::Pause;SmokeYaw=L->BarkCount;Key(EKeys::B,true);Next();}break;
    case 12:
        if(SmokeTime>.2){Key(EKeys::B,false);SmokeCheck(L->BarkCount==SmokeYaw,TEXT("Pause suppresses bark input"));Next();}break;
    case 13:
        if(SmokeTime>.5)
        {
            S->Screen=ECozyScreen::Playing;D->Music=0;D->Effects=.6f;S->UpdateMusic();
            P->ReturnHome();P->TeleportTo(FVector(70,-310,35),FRotator::ZeroRotator);Next();
        }break;
    case 14:
        if(SmokeTime>1){Record();SmokeYaw=S->AudibleCueCount;Key(EKeys::E,true);Next();}break;
    case 15:
        if(SmokeTime>.1){Key(EKeys::E,false);SmokeCheck(D->CarryItem==RainbowGuyItem&&S->LastCue==TEXT("RainbowSqueak")&&S->AudibleCueCount==SmokeYaw+1,TEXT("Biting Rainbow Guy on keyboard pickup plays one squeak"));Next();}break;
    case 16:
        if(SmokeTime>.8)
        {
            Stop(TEXT("RainbowSqueak"));L->Stow();SmokeYaw=S->AudibleCueCount;L->Retrieve();
            SmokeCheck(D->CarryItem==RainbowGuyItem&&S->LastCue==TEXT("RainbowSqueak")&&S->AudibleCueCount==SmokeYaw+1,TEXT("Taking Rainbow Guy from the backpack squeaks in Marty's mouth"));
            L->Drop();SmokeCheck(S->LastCue==TEXT("Toy"),TEXT("Dropping Rainbow Guy uses the ordinary toy cue without a bite squeak"));
            D->Effects=0;SmokeYaw=S->AudibleCueCount;L->Interact();
            SmokeCheck(D->CarryItem==RainbowGuyItem&&S->AudibleCueCount==SmokeYaw,TEXT("Effects volume zero mutes Rainbow Guy's bite squeak"));
            L->Stow();D->Effects=.6f;S->ToggleFeature(ECozyFeature::Effects);SmokeYaw=S->AudibleCueCount;L->Retrieve();
            SmokeCheck(D->CarryItem==RainbowGuyItem&&S->AudibleCueCount==SmokeYaw,TEXT("Effects feature off suppresses Rainbow Guy's bite squeak"));
            S->ToggleFeature(ECozyFeature::Effects);Next();
        }break;
    case 17:
        if(SmokeTime>3)
        {
            auto O=MakeShared<FJsonObject>();TArray<TSharedPtr<FJsonValue>> Pass,Fail;for(auto& V:SmokePassed)Pass.Add(MakeShared<FJsonValueString>(V));for(auto& V:SmokeFailed)Fail.Add(MakeShared<FJsonValueString>(V));
            O->SetStringField(TEXT("scope"),TEXT("Synthetic keyboard/gamepad and real audio-device master-submix recordings. Physical controls and subjective listening unverified."));O->SetArrayField(TEXT("passed"),Pass);O->SetArrayField(TEXT("failed"),Fail);
            FString Json;FJsonSerializer::Serialize(O,TJsonWriterFactory<>::Create(&Json));bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/TEXT("audio-smoke.json")));UGameplayStatics::DeleteGameInSlot(TEXT("CheweenieAudioAcceptance"),0);FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);SmokeStage=1000;
        }break;
    }
}
