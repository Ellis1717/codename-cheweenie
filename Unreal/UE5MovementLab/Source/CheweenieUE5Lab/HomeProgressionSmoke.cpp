#include "MartyCharacter.h"
#include "HomeProgression.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "CatCompanions.h"
#include "DogLife.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "InputKeyEventArgs.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"
#include "Misc/App.h"
#include "UnrealClient.h"

void AMartyPlayMode::TickHomeProgressionSmoke(float Delta)
{
    SmokeTime+=Delta;auto* PC=GetWorld()->GetFirstPlayerController();auto* P=PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;
    auto* S=Adventure.Get();auto* H=HomeProgression.Get();if(!P||!S||!S->Progress||!H)return;H->Initialize();auto* D=S->Progress.Get();auto* L=P->Life.Get();
    FString Root=FPaths::ProjectSavedDir();FParse::Value(FCommandLine::Get(),TEXT("CheweenieTestOutput="),Root);
    auto Key=[&](FKey K,bool Down){PC->InputKey(FInputKeyEventArgs(nullptr,INPUTDEVICEID_NONE,K,Down?IE_Pressed:IE_Released,FPlatformTime::Cycles64()));};
    auto Place=[&](FVector V){L->CancelAction();P->GetCharacterMovement()->StopMovementImmediately();P->GetCharacterMovement()->SetMovementMode(MOVE_Walking);V.Z=28;P->TeleportTo(V,FRotator::ZeroRotator);PC->SetControlRotation(FRotator(-12,0,0));};
    auto Next=[&](){++SmokeStage;SmokeTime=0;};auto Shot=[&](const TCHAR* File){if(FApp::CanEverRender())FScreenshotRequest::RequestScreenshot(Root/File,true,false);};
    const bool Resume=FParse::Param(FCommandLine::Get(),TEXT("CheweenieHomeResume"));
    auto Finish=[&](){auto J=MakeShared<FJsonObject>();auto Strings=[](const TArray<FString>& In){TArray<TSharedPtr<FJsonValue>> Out;for(const auto& V:In)Out.Add(MakeShared<FJsonValueString>(V));return Out;};J->SetStringField(TEXT("scope"),TEXT("Actual home/annex map, synthetic keyboard/gamepad, real station/trick actions; position, currency, cooldown and legacy-save fixtures. Physical controllers unverified."));J->SetArrayField(TEXT("passed"),Strings(SmokePassed));J->SetArrayField(TEXT("failed"),Strings(SmokeFailed));FString Json;FJsonSerializer::Serialize(J,TJsonWriterFactory<>::Create(&Json));bool Saved=FFileHelper::SaveStringToFile(Json,*(Root/(Resume?TEXT("home-resume.json"):TEXT("home-progression.json"))));if(Resume)UGameplayStatics::DeleteGameInSlot(S->Slot,0);SmokeStage=999;FPlatformMisc::RequestExitWithStatus(false,Saved&&SmokeFailed.IsEmpty()?0:1);};
    if(Resume){if(SmokeStage==0&&SmokeTime>3){SmokeCheck(D->ArmorLevel==1&&D->StrengthLevel==1&&D->AgilityLevel==1&&D->SpeedLevel==1,TEXT("All four upgrade tiers persist in another process"));SmokeCheck(D->PraiseStars==7&&D->TrickPraiseMask==2048,TEXT("Currency and trick-variety ledger persist"));SmokeCheck(D->MealBonusRemaining==60&&D->RestBonusRemaining==90&&D->PraiseCooldownRemaining==5,TEXT("Saved bonus/cooldown durations do not tick on the title screen"));SmokeCheck(!H->Resting&&H->Training<0&&L->Activity==EDogActivity::Idle,TEXT("Resting and training never resume as stuck transient actions"));SmokeCheck(FMath::IsNearlyEqual(H->SpeedMultiplier(),1.05f)&&FMath::IsNearlyEqual(H->DamageMultiplier(),.85f),TEXT("Reloaded upgrades have real movement/protection effects"));Finish();}return;}
    // Separate interaction releases from the next Started edge, including mixed keyboard/gamepad input.
    if(SmokeTime>.05&&SmokeTime<.2){Key(EKeys::E,false);Key(EKeys::Gamepad_FaceButton_Left,false);}
    if(SmokeTime<.2)return;
    switch(SmokeStage)
    {
    case 0:
        if(SmokeTime<(FApp::CanEverRender()?35:3))break;S->Start(false);D->CatFollowMask=0;D->CostumeMask=0;D->PetPace=0;SmokeYaw=D->Music;SmokePitch=D->SnackMask;
        SmokeCheck(S->Slot==TEXT("CheweenieHomeAcceptance"),TEXT("Home acceptance uses an isolated slot"));SmokeCheck(D->PraiseStars==3&&H->Level(0)==0&&H->Level(3)==0,TEXT("Older saves acquire compatible starter stars and zero upgrade tiers"));
        SmokeCheck(H->Enabled()&&H->Prompt().IsEmpty(),TEXT("Home progression defaults on without hijacking the spawn interaction"));
        Place(Companions->Position(0)+FVector(-95,0,0));Key(EKeys::B,true);Next();break;
    case 1:
        Key(EKeys::B,false);SmokeCheck(D->PraiseStars==4&&S->Message.Contains(TEXT("Lakitu")),TEXT("A real nearby bark earns a named cat reaction and a praise star"));SmokeCheck(Companions->Cats[0].Happy>0&&H->Reactions==1,TEXT("Cat response activates visible happy feedback"));Shot(TEXT("HomeCatReaction.png"));Next();break;
    case 2:
        if(SmokeTime<1.4)break;D->PraiseCooldownRemaining=0;Key(EKeys::B,true);Next();break;
    case 3:
        Key(EKeys::B,false);SmokeCheck(D->PraiseStars==4,TEXT("Repeating the same trick cannot farm praise stars"));D->PraiseCooldownRemaining=0;Key(EKeys::LeftControl,true);Next();break;
    case 4:Key(EKeys::One,true);Next();break;
    case 5:
        Key(EKeys::One,false);SmokeCheck(D->PraiseStars==5&&L->Activity==EDogActivity::Lying,TEXT("Direct lie receives its own contextual reaction"));L->StandUp();D->PraiseCooldownRemaining=0;Key(EKeys::Two,true);Next();break;
    case 6:Key(EKeys::Two,false);Next();break;
    case 7:
        if(SmokeTime<2.7)break;SmokeCheck(D->PraiseStars==6,TEXT("A completed roll receives praise after its animation"));D->PraiseCooldownRemaining=0;Key(EKeys::Three,true);Next();break;
    case 8:Key(EKeys::Three,false);Next();break;
    case 9:
        if(SmokeTime<2)break;Key(EKeys::LeftControl,false);SmokeCheck(D->PraiseStars==9&&(D->TrickPraiseMask&15)==0,TEXT("Four varied tricks complete a round with a bounded two-star bonus"));
        Place(H->Bed);D->PetEnergy=20;D->PetHealth=60;Key(EKeys::E,true);Next();break;
    case 10:
        Key(EKeys::E,false);SmokeCheck(H->Resting&&L->Activity==EDogActivity::Lying,TEXT("Bed interaction starts actual rest without a trick menu"));Next();break;
    case 11:
        if(SmokeTime<3.2)break;SmokeCheck(D->PetEnergy>35&&D->PetHealth>64&&D->RestBonusRemaining>170,TEXT("Bed restores needs and grants the Rested bonus after three seconds"));Shot(TEXT("HomeBedRest.png"));Key(EKeys::W,true);Next();break;
    case 12:
        Key(EKeys::W,false);SmokeCheck(!H->Resting&&L->Activity==EDogActivity::Idle,TEXT("Movement gets Marty out of bed normally"));Place(L->Bowl+FVector(-35,0,0));D->PetEnergy=20;Key(EKeys::E,true);Next();break;
    case 13:Key(EKeys::E,false);Next();break;
    case 14:
        if(SmokeTime<1.8)break;SmokeCheck(D->PetEnergy>=59&&D->MealBonusRemaining>115,TEXT("The real kitchen meal grants extra energy and the food bonus"));SmokeCheck(H->DrainMultiplier()<.65f,TEXT("Food and Rested bonuses combine with a bounded energy-drain reduction"));
        S->Pause();SmokePosition=FVector(D->MealBonusRemaining,D->RestBonusRemaining,D->PraiseCooldownRemaining);Next();break;
    case 15:
        if(SmokeTime<.6)break;SmokeCheck(FMath::IsNearlyEqual(D->MealBonusRemaining,float(SmokePosition.X),.001f)&&FMath::IsNearlyEqual(D->RestBonusRemaining,float(SmokePosition.Y),.001f),TEXT("Pause freezes home bonus timers"));S->Back();Place(H->Portal);Key(EKeys::Gamepad_FaceButton_Left,true);Next();break;
    case 16:
        Key(EKeys::Gamepad_FaceButton_Left,false);SmokeCheck(H->InAnnex()&&Navigation->Location().Contains(TEXT("garage")),TEXT("Controller interaction enters the real garage/gym annex"));
        SmokeCheck(P->GetCharacterMovement()->IsMovingOnGround(),TEXT("The annex has a real walkable floor"));Place(H->ArmorBench+FVector(-75,0,0));D->PraiseStars=20;Key(EKeys::E,true);Next();break;
    case 17:
        Key(EKeys::E,false);SmokeCheck(D->ArmorLevel==1&&D->PraiseStars==17,TEXT("Garage spends the shown star cost for a saved armor tier"));SmokeCheck(H->GearVisible(),TEXT("The padded harness is actually visible on Marty"));Shot(TEXT("HomeArmorWorkshop.png"));D->PetHealth=100;Place(H->BumpTest+FVector(-70,0,0));Key(EKeys::E,true);Next();break;
    case 18:
        Key(EKeys::E,false);SmokeCheck(D->PetHealth>91&&D->PetHealth<92,TEXT("Foam safety test demonstrates real armor damage mitigation"));Place(H->Gym[0]);D->PraiseCooldownRemaining=0;Key(EKeys::E,true);Next();break;
    case 19:
        Key(EKeys::E,false);SmokeCheck(H->Training==0&&D->StrengthLevel==0,TEXT("Strength requires an actual training task before an upgrade"));Key(EKeys::LeftControl,true);Next();break;
    case 20:Key(EKeys::Three,true);Next();break;
    case 21:Key(EKeys::Three,false);Next();break;
    case 22:
        if(SmokeTime<2)break;Key(EKeys::LeftControl,false);SmokeCheck(D->StrengthLevel==1&&H->Training<0,TEXT("Completing the shake earns the strength tier"));SmokeCheck(H->DigCost()<6&&H->DigDuration()<.9f,TEXT("Strength changes actual digging energy cost and duration"));Place(H->Gym[1]);D->PraiseCooldownRemaining=0;Key(EKeys::E,true);Next();break;
    case 23:Key(EKeys::E,false);Key(EKeys::LeftControl,true);Next();break;
    case 24:Key(EKeys::Two,true);Next();break;
    case 25:Key(EKeys::Two,false);Next();break;
    case 26:
        if(SmokeTime<2.7)break;Key(EKeys::LeftControl,false);SmokeCheck(D->AgilityLevel==1&&H->JumpMultiplier()>1,TEXT("Completing the roll earns a real jump-height upgrade"));Place(H->Gym[2]);D->PetEnergy=90;Key(EKeys::E,true);Next();break;
    case 27:
        Key(EKeys::E,false);SmokeCheck(H->Training==2&&Navigation->Goal().Label.Contains(TEXT("Speed")),TEXT("Speed training temporarily guides the player to its finish"));Key(EKeys::LeftShift,true);Key(EKeys::W,true);Next();break;
    case 28:
        if(SmokeTime<2.4)break;Key(EKeys::W,false);Key(EKeys::LeftShift,false);SmokeCheck(D->SpeedLevel==1&&H->Training<0,TEXT("Running through the actual finish earns the speed tier"));Shot(TEXT("HomeGym.png"));Next();break;
    case 29:
        SmokeCheck(L->WalkSpeed()>100&&L->WalkSpeed()<110,TEXT("Speed tier changes the real movement speed"));S->ToggleFeature(ECozyFeature::HomeProgression);
        SmokeCheck(H->SpeedMultiplier()==1&&H->DamageMultiplier()==1&&H->DrainMultiplier()==1,TEXT("Disabling progression removes perk effects without deleting levels"));SmokeCheck(D->ArmorLevel==1&&D->StrengthLevel==1,TEXT("Disabled progression preserves purchased upgrades"));Place(H->Exit);Key(EKeys::E,true);Next();break;
    case 30:
        Key(EKeys::E,false);SmokeCheck(!H->InAnnex()&&P->GetActorLocation().X<1000,TEXT("The return door works even with progression disabled"));S->ToggleFeature(ECozyFeature::HomeProgression);D->PraiseStars=0;Place(H->Portal);H->Travel(true);Place(H->ArmorBench+FVector(-75,0,0));Key(EKeys::E,true);Next();break;
    case 31:
        Key(EKeys::E,false);SmokeCheck(D->ArmorLevel==1&&D->PraiseStars==0,TEXT("Insufficient funds cannot buy or overdraw an upgrade"));D->ArmorLevel=3;D->PraiseStars=20;Next();break;
    case 32:Key(EKeys::E,true);Next();break;
    case 33:
        Key(EKeys::E,false);SmokeCheck(D->ArmorLevel==3&&D->PraiseStars==20,TEXT("Maximum-tier interaction never spends more stars"));D->ArmorLevel=1;Place(H->Gym[0]);Key(EKeys::E,true);Next();break;
    case 34:
        Key(EKeys::E,false);P->ReturnHome();SmokeCheck(H->Training<0&&!H->InAnnex(),TEXT("Home recovery safely cancels unfinished station training"));
        SmokeCheck(D->SnackMask==int32(SmokePitch)&&FMath::IsNearlyEqual(D->Music,SmokeYaw),TEXT("Home progression preserves existing patrol and sound preferences"));
        D->PraiseStars=7;D->TrickPraiseMask=2048;D->MealBonusRemaining=60;D->RestBonusRemaining=90;D->PraiseCooldownRemaining=5;S->Pause();S->Save();Next();break;
    case 35:Finish();break;
    }
}
