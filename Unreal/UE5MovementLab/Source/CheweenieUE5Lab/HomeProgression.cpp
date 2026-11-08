#include "HomeProgression.h"
#include "ControllerGuide.h"
#include "HouseUpstairs.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "MartyCharacter.h"
#include "MartyGait.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "DogLife.h"
#include "Neighbourhood.h"
#include "CatCompanions.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkinnedAsset.h"
#include "Engine/DamageEvents.h"
#include "Materials/MaterialInstanceDynamic.h"

UHomeProgression* CheweenieHome(const UObject* C){auto* M=C&&C->GetWorld()?C->GetWorld()->GetAuthGameMode<AMartyPlayMode>():nullptr;return M?M->HomeProgression.Get():nullptr;}
UHomeProgression::UHomeProgression(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickGroup=TG_PostUpdateWork;}
AMartyPlayMode* UHomeProgression::Mode() const{return Cast<AMartyPlayMode>(GetOwner());}
AMartyCharacter* UHomeProgression::Marty() const{auto* PC=GetWorld()->GetFirstPlayerController();return PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;}
UCozyAdventureSave* UHomeProgression::Data() const{return Mode()&&Mode()->Adventure?Mode()->Adventure->Progress.Get():nullptr;}
bool UHomeProgression::Enabled() const{return Data()&&Mode()->Adventure->Enabled&&Mode()->Adventure->Feature(ECozyFeature::HomeProgression);}
bool UHomeProgression::Playing() const{return Data()&&Mode()->Adventure->Enabled&&Mode()->Adventure->Screen==ECozyScreen::Playing;}
bool UHomeProgression::InAnnex() const{return Marty()&&PointInAnnex(Marty()->GetActorLocation());}
int32 UHomeProgression::Level(int32 Type) const{const auto* D=Data();if(!D)return 0;return FMath::Clamp(Type==0?D->StrengthLevel:Type==1?D->AgilityLevel:Type==2?D->SpeedLevel:D->ArmorLevel,0,3);}
int32 UHomeProgression::UpgradeCost(int32 Type) const{return (Type==3?3:2)+Level(Type)*2;}
float UHomeProgression::SpeedMultiplier() const{return Enabled()?1+.05f*Level(2):1;}
float UHomeProgression::JumpMultiplier() const{return Enabled()?1+.06f*Level(1):1;}
float UHomeProgression::DamageMultiplier() const{return Enabled()?1-.15f*Level(3):1;}
float UHomeProgression::DrainMultiplier() const{return !Enabled()?1:(Data()->MealBonusRemaining>0?.75f:1)*(Data()->RestBonusRemaining>0?.85f:1);}
float UHomeProgression::DigDuration() const{return Enabled()?.9f-.12f*Level(0):.9f;}
float UHomeProgression::DigCost() const{return Enabled()?6-.75f*Level(0):6;}
void UHomeProgression::Tell(const FString& Text,const FString& Sound){auto* S=Mode()->Adventure.Get();S->Message=Text;S->HoldMessage(6);S->ActionCue(Sound);}
void UHomeProgression::Initialize()
{
    if(Ready||!Data()||!Marty()||!Mode()->Adventure->Enabled||!Mode()->Neighbourhood||Mode()->Neighbourhood->Houses<9)return;
    auto* D=Data();D->PraiseStars=FMath::Clamp(D->PraiseStars,0,99);D->TrickPraiseMask&=4095;
    for(int32* L:{&D->StrengthLevel,&D->AgilityLevel,&D->SpeedLevel,&D->ArmorLevel})*L=FMath::Clamp(*L,0,3);
    D->MealBonusRemaining=FMath::IsFinite(D->MealBonusRemaining)?FMath::Clamp(D->MealBonusRemaining,0.f,MealDuration):0;
    D->RestBonusRemaining=FMath::IsFinite(D->RestBonusRemaining)?FMath::Clamp(D->RestBonusRemaining,0.f,RestDuration):0;
    D->PraiseCooldownRemaining=FMath::IsFinite(D->PraiseCooldownRemaining)?FMath::Clamp(D->PraiseCooldownRemaining,0.f,PraiseCooldown):0;
    Build();if(Marty()->Gait)AddTickPrerequisiteComponent(Marty()->Gait);Ready=true;
}
AActor* UHomeProgression::Shape(const TCHAR* Tag,FVector P,FVector Size,int32 Color,bool Solid,const TCHAR* Mesh)
{
    auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(P,FRotator::ZeroRotator);auto* C=A->GetStaticMeshComponent();C->SetMobility(EComponentMobility::Movable);
    C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"),Mesh,Mesh)));C->SetMaterial(0,Palette[Color]);A->SetActorScale3D(Size/100);
    C->SetCollisionEnabled(Solid?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);if(Solid)C->SetCollisionProfileName(TEXT("BlockAll"));C->SetCanEverAffectNavigation(false);A->Tags.Add(FName(Tag));return A;
}
void UHomeProgression::Sign(const FString& Text,FVector P,float Size)
{
    auto* A=GetWorld()->SpawnActor<AActor>();auto* C=NewObject<UTextRenderComponent>(A);A->SetRootComponent(C);C->RegisterComponent();C->SetText(FText::FromString(Text));C->SetWorldSize(Size);C->SetTextRenderColor(FColor(255,229,163));C->SetHorizontalAlignment(EHTA_Center);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);A->SetActorLocationAndRotation(P,FRotator(0,180,0));
    if(Text.Contains(TEXT("E / X-Square"))){InputSigns.Add(C);InputSignSources.Add(Text);SignFamily=-1;}
}
void UHomeProgression::Build()
{
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    for(const auto Color:{FLinearColor(.07f,.13f,.16f),FLinearColor(.15f,.42f,.43f),FLinearColor(.55f,.27f,.08f),FLinearColor(.36f,.39f,.36f),FLinearColor(.8f,.62f,.2f)}){auto* M=UMaterialInstanceDynamic::Create(Base,this);M->SetVectorParameterValue(TEXT("Color"),Color);Palette.Add(M);}
    Shape(TEXT("CozyHomeBed"),Bed+FVector(0,0,5),FVector(115,80,10),1,false,TEXT("Sphere"));Shape(TEXT("CozyHomePillow"),Bed+FVector(-35,0,12),FVector(30,60,13),4,false,TEXT("Sphere"));Sign(TEXT("MARTY'S BED\nRest + recover"),Bed+FVector(-40,0,85),16);
    for(float Y:{-50.f,50.f})Shape(TEXT("CozyHomeDoor"),Portal+FVector(30,Y,60),FVector(6,8,150),1);Shape(TEXT("CozyHomeDoor"),Portal+FVector(30,0,135),FVector(6,108,8),1);Sign(TEXT("GARAGE + GYM\nE / X-Square"),Portal+FVector(20,0,120),18);
    Shape(TEXT("CozyHomeFloor"),FVector(8300,0,-10),FVector(1400,800,20),3,true);
    for(float X:{7600.f,9000.f})Shape(TEXT("CozyHomeWall"),FVector(X,0,140),FVector(20,800,280),0,true);
    for(float Y:{-400.f,400.f})Shape(TEXT("CozyHomeWall"),FVector(8300,Y,140),FVector(1400,20,280),0,true);
    for(float Y:{-255.f,255.f})Shape(TEXT("CozyHomePartition"),FVector(8300,Y,140),FVector(16,290,280),1,true);
    for(float X:{7950.f,8650.f}){auto* A=GetWorld()->SpawnActor<AActor>();auto* C=NewObject<UPointLightComponent>(A);A->SetRootComponent(C);C->RegisterComponent();C->SetIntensity(5500);C->SetAttenuationRadius(700);C->SetCastShadows(false);A->SetActorLocation(FVector(X,0,230));}
    Sign(TEXT("GARAGE\nSNUGGLE SHIELD WORKSHOP"),FVector(7980,330,145),26);Sign(TEXT("GYM\nCOACH KEETZ'S PRACTICE CORNER"),FVector(8660,330,145),24);
    for(float Y:{-50.f,50.f})Shape(TEXT("CozyHomeReturn"),Exit+FVector(-40,Y,40),FVector(6,8,140),1);Shape(TEXT("CozyHomeReturn"),Exit+FVector(-40,0,110),FVector(6,108,8),1);Sign(TEXT("BACK TO HEADQUARTERS\nE / X-Square"),Exit+FVector(-30,0,100),17);
    Shape(TEXT("CozyHomeArmorBench"),ArmorBench+FVector(0,0,24),FVector(110,70,48),2);Sign(TEXT("PADDED HARNESS\n15% protection / tier"),ArmorBench+FVector(0,0,95),17);
    Shape(TEXT("CozyHomeFoamTest"),BumpTest+FVector(0,0,38),FVector(55,55,75),4,false,TEXT("Sphere"));Sign(TEXT("FOAM SAFETY TEST\n10 test damage; armor helps"),BumpTest+FVector(0,0,105),16);
    const TCHAR* Names[]={TEXT("STRENGTH\nE / X-Square then shake a paw"),TEXT("AGILITY\nE / X-Square then roll over"),TEXT("SPEED\nE / X-Square then run to the gold finish")};
    for(int32 I=0;I<3;++I){Shape(TEXT("CozyHomeTrainingMat"),Gym[I]+FVector(0,0,2),FVector(130,100,4),1);Sign(Names[I],Gym[I]+FVector(0,0,90),17);}
    for(int32 I=0;I<9;++I)Shape(TEXT("CozyHomeRunLane"),FVector(8400+I*50,180,2),FVector(22,6,3),4);
    Shape(TEXT("CozyHomeFinish"),Finish-FVector(0,0,25),FVector(25,130,5),4);Sign(TEXT("FINISH\nRun through here"),Finish+FVector(15,0,70),17);
    ReactionLabels.SetNum(2);Mode()->Companions->Initialize();
    for(int32 I=0;I<2;++I)if(auto* A=Mode()->Companions->Cats[I].Actor.Get()){auto* T=NewObject<UTextRenderComponent>(A);T->SetupAttachment(A->GetRootComponent());T->RegisterComponent();T->SetRelativeLocation(FVector(0,0,160));T->SetRelativeScale3D(FVector(3.125));T->SetWorldSize(14);T->SetHorizontalAlignment(EHTA_Center);T->SetTextRenderColor(FColor(255,220,90));T->SetVisibility(false);T->SetCollisionEnabled(ECollisionEnabled::NoCollision);ReactionLabels[I]=T;}
    Gear=GetWorld()->SpawnActor<AActor>();auto* Root=NewObject<USceneComponent>(Gear);Gear->SetRootComponent(Root);Root->RegisterComponent();
    for(int32 I=0;I<3;++I){auto* C=NewObject<UStaticMeshComponent>(Gear);C->SetupAttachment(Root);C->RegisterComponent();C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));C->SetMaterial(0,Palette[1]);C->SetRelativeLocation(I==0?FVector(0,0,5):FVector(0,I==1?-9:9,0));C->SetRelativeScale3D((I==0?FVector(25,19,5):FVector(20,5,14))/100);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCanEverAffectNavigation(false);GearPieces.Add(C);}Gear->SetActorHiddenInGame(true);
}
FString UHomeProgression::Summary() const{return !Enabled()?TEXT("Home upgrades: OFF"):FString::Printf(TEXT("Stars %d | Shield %d/3 (%d%%)"),Data()->PraiseStars,Level(3),Level(3)*15);}
FString UHomeProgression::BuffSummary() const{return !Enabled()?FString():FString::Printf(TEXT("Meal %ds | Rested %ds"),FMath::CeilToInt(Data()->MealBonusRemaining),FMath::CeilToInt(Data()->RestBonusRemaining));}
FString UHomeProgression::TrainingObjective() const{return Training==0?TEXT("Strength: finish a shake near the teal mat (L1/LB + Square/X)"):Training==1?TEXT("Agility: finish a roll near the teal mat (L1/LB + Circle/B)"):Training==2?TEXT("Speed: hurry through the gold finish (A/Cross without L1/LB)"):FString();}
FVector UHomeProgression::TrainingGoal() const{return Training==2?Finish:Training>=0?Gym[Training]:Portal;}
FString UHomeProgression::Prompt() const
{
    if(!Playing()||!Marty())return FString();const FVector P=Marty()->GetActorLocation();
    if(InAnnex()&&CheweenieHouse::Near(P,Exit,85))return TEXT("E / X-Square: return to headquarters");
    if(!Mode()->Neighbourhood->Outside&&!InAnnex()&&CheweenieHouse::Near(P,Portal,85))return TEXT("E / X-Square: enter the garage and gym");
    if(InAnnex()&&FVector::Dist2D(P,BumpTest)<85)return TEXT("E / X-Square: foam harness test (10 test damage, reduced by armor)");
    if(Training>=0)return TrainingObjective()+FString::Printf(TEXT(" | %.0fs"),TrainingRemaining);
    int32 Station=-1;if(!Mode()->Neighbourhood->Outside&&CheweenieHouse::Near(P,Bed,85))Station=4;
    if(InAnnex()){if(FVector::Dist2D(P,ArmorBench)<90)Station=3;for(int32 I=0;I<3;++I)if(FVector::Dist2D(P,Gym[I])<85)Station=I;}
    if(Station<0)return FString();if(!Enabled())return TEXT("Home upgrades are OFF. Enable Controls > Pet feature switches.");
    if(Station==4)return Resting?TEXT("Resting in Marty's bed | E / X-Square or movement: get up"):TEXT("E / X-Square: rest in Marty's bed (energy + health)");
    const TCHAR* Names[]={TEXT("strength"),TEXT("agility"),TEXT("speed"),TEXT("padded harness")};
    return Level(Station)>=3?FString(Names[Station])+TEXT(": maximum tier 3 reached"):FString::Printf(TEXT("E / X-Square: %s %s tier %d | %d stars (you have %d)"),Station==3?TEXT("upgrade"):TEXT("train"),Names[Station],Level(Station)+1,UpgradeCost(Station),Data()->PraiseStars);
}
void UHomeProgression::Travel(bool Enter)
{
    auto* P=Marty();if(!P)return;P->ClearControllerInput();CancelActivities();P->Life->CancelAction();P->GetCharacterMovement()->StopMovementImmediately();Mode()->Neighbourhood->Outside=false;Mode()->Neighbourhood->SetOutdoorLighting(false);
    P->TeleportTo(Enter?FVector(7850,-80,28):Portal+FVector(-100,0,0),FRotator::ZeroRotator);if(auto* PC=Cast<APlayerController>(P->GetController()))PC->SetControlRotation(FRotator(-12,0,0));Mode()->Companions->OnPlayerTravel(false);Mode()->Navigation->RefreshRoute();Tell(Enter?TEXT("Garage + gym: earn praise stars with varied tricks, then upgrade or train."):TEXT("Back at cozy headquarters. Your upgrades are kept."));
}
bool UHomeProgression::Interact()
{
    Initialize();if(!Playing()||!Ready||Marty()->Life->Busy())return false;auto* P=Marty();const FVector Here=P->GetActorLocation();
    if(InAnnex()&&CheweenieHouse::Near(Here,Exit,85)){Travel(false);return true;}
    if(!Mode()->Neighbourhood->Outside&&!InAnnex()&&CheweenieHouse::Near(Here,Portal,85)){Travel(true);return true;}
    if(InAnnex()&&FVector::Dist2D(Here,BumpTest)<85){if(!Enabled()||!Mode()->Adventure->Feature(ECozyFeature::Health)){Tell(TEXT("Enable home upgrades and health to try the foam safety test."));return true;}if(BumpCooldown>0){Tell(TEXT("The foam tester is resetting. No extra damage taken."));return true;}const float Before=Data()->PetHealth;P->TakeDamage(10,FDamageEvent(),nullptr,nullptr);BumpCooldown=6;Tell(FString::Printf(TEXT("Foam test: %.1f damage taken; shield blocked %.1f. Bed rest or food restores health."),Before-Data()->PetHealth,10-(Before-Data()->PetHealth)));return true;}
    int32 Station=-1;if(!Mode()->Neighbourhood->Outside&&CheweenieHouse::Near(Here,Bed,85))Station=4;
    if(InAnnex()){if(FVector::Dist2D(Here,ArmorBench)<90)Station=3;for(int32 I=0;I<3;++I)if(FVector::Dist2D(Here,Gym[I])<85)Station=I;}
    if(Station<0)return false;if(!Enabled()){Tell(TEXT("Enable Home upgrades & reactions in Controls > Pet feature switches."));return true;}
    if(Station==4){if(P->GetCharacterMovement()->IsFalling())return true;P->Life->StopWatching();P->Life->Activity=EDogActivity::Lying;P->GetCharacterMovement()->StopMovementImmediately();Resting=true;RestTime=0;Tell(TEXT("Cheeween Cozy: a proper bed break. Rest three seconds for a Rested bonus."),TEXT("Cozy"));return true;}
    if(Level(Station)>=3){Tell(TEXT("Tier 3 reached. Marty is officially overqualified for this station."));return true;}
    if(Data()->PraiseStars<UpgradeCost(Station)){Tell(TEXT("More praise stars needed. Show the cats four different tricks, or practice in the gym."));return true;}
    if(Station==3){Data()->PraiseStars-=UpgradeCost(3);++Data()->ArmorLevel;Mode()->Adventure->Save();Tell(TEXT("Snuggle Shield fitted! A padded harness reduces incoming damage by 15% per tier."),TEXT("Discovery"));return true;}
    if((Station<2&&!Mode()->Adventure->Feature(ECozyFeature::Tricks))||(Station==2&&!Mode()->Adventure->Feature(ECozyFeature::Hurry))){Tell(TEXT("Enable Dog tricks or Jog/run in Pet feature switches for this session."));return true;}
    if(Mode()->Adventure->Feature(ECozyFeature::Energy)&&Data()->PetEnergy<30){Tell(TEXT("Training needs 30 energy to start. Rest in bed or eat in the kitchen first."));return true;}
    Training=Station;TrainingRemaining=30;TrainingSpeed=0;TrainingStart=Here;Tell(TrainingObjective());return true;
}
void UHomeProgression::CancelActivities(){Resting=false;RestTime=0;Training=-1;TrainingRemaining=0;TrainingSpeed=0;}
void UHomeProgression::FinishTraining()
{
    if(Training<0||!Enabled())return;const int32 T=Training,Cost=UpgradeCost(T);Training=-1;TrainingRemaining=0;
    if(Level(T)>=3||Data()->PraiseStars<Cost){Tell(TEXT("Session complete, but more stars are needed before upgrading. No stars spent."));return;}
    if(Mode()->Adventure->Feature(ECozyFeature::Energy)){if(Data()->PetEnergy<8){Tell(TEXT("Marty needs a rest before this upgrade. No stars spent."));return;}Data()->PetEnergy-=8;}
    Data()->PraiseStars-=Cost;int32& L=T==0?Data()->StrengthLevel:T==1?Data()->AgilityLevel:Data()->SpeedLevel;++L;Mode()->Adventure->Save();Mode()->Navigation->RefreshRoute();
    Tell(FString::Printf(TEXT("Coach Keetz: %s tier %d earned! %s"),T==0?TEXT("Strength"):T==1?TEXT("Agility"):TEXT("Speed"),L,T==0?TEXT("Quicker digging, less dig energy."):T==1?TEXT("Higher jumps."):TEXT("Faster walking, jogging and running.")),TEXT("Discovery"));
}
void UHomeProgression::Praise(int32 Witness,int32 Skill)
{
    auto* D=Data();if(!D||D->PraiseCooldownRemaining>0)return;const int32 Bit=1<<(Witness*4+Skill);const bool Fresh=(D->TrickPraiseMask&Bit)==0;
    const TCHAR* Names[]={TEXT("Lakitu"),TEXT("Rio"),TEXT("Coach Keetz")};
    const TCHAR* Lines[]={TEXT("Excellent blanket form. A very horizontal professional."),TEXT("Laundry cycle approved. Please do not tumble-dry the dog."),TEXT("A firm pawshake. Your snack application is under review."),TEXT("Volume checked. Still a dog. Strong work.")};
    int32 Award=0;if(Fresh){D->TrickPraiseMask|=Bit;Award=1;const int32 Round=15<<(Witness*4);if((D->TrickPraiseMask&Round)==Round){D->TrickPraiseMask&=~Round;Award+=2;}D->PraiseStars=FMath::Min(99,D->PraiseStars+Award);}
    D->PraiseCooldownRemaining=PraiseCooldown;++Reactions;
    if(Witness<2){Mode()->Companions->Cats[Witness].Happy=5;ReactionTime[Witness]=4;if(ReactionLabels.IsValidIndex(Witness)&&ReactionLabels[Witness]){ReactionLabels[Witness]->SetText(FText::FromString(Award?FString::Printf(TEXT("APPROVED! +%d STAR%s"),Award,Award==1?TEXT(""):TEXT("S")):TEXT("SHOW ME ANOTHER TRICK!")));ReactionLabels[Witness]->SetVisibility(Mode()->Adventure->Feature(ECozyFeature::WorldLabels)&&GetWorld()->GetGameInstance()->GetSubsystem<UCozyModeHub>()->OverlayVisible(ECozyOverlay::Reactions));}}
    Tell(FString::Printf(TEXT("%s: %s %s"),Names[Witness],Lines[Skill],Award?*FString::Printf(TEXT("+%d praise star%s!"),Award,Award==1?TEXT(""):TEXT("s")):TEXT("Try a different trick for another star.")),Witness==0?TEXT("Lakitu"):Witness==1?TEXT("Rio"):TEXT("Cozy"));Mode()->Adventure->Save();
}
void UHomeProgression::OnSkill(int32 Skill)
{
    if(!Playing()||!Enabled()||Skill<0||Skill>3)return;
    auto* C=Mode()->Companions.Get();int32 Witness=-1;
    if(C&&C->Enabled())for(int32 I=0;I<2;++I)if(C->Cats[I].Actor.IsValid()&&C->Outdoors(I)==Mode()->Neighbourhood->Outside&&CheweenieHouse::Near(Marty()->GetActorLocation(),C->Position(I),180)){Witness=I;break;}
    if(Witness<0&&InAnnex()&&Marty()->GetActorLocation().X>8320)Witness=2;
    if(Witness>=0)Praise(Witness,Skill);
    if(Training>=0&&Training<2&&Skill==(Training==0?2:1)&&FVector::Dist2D(Marty()->GetActorLocation(),Gym[Training])<160)FinishTraining();
}
void UHomeProgression::OnMeal(){if(!Enabled())return;Data()->MealBonusRemaining=MealDuration;if(Mode()->Adventure->Feature(ECozyFeature::Energy))Data()->PetEnergy=FMath::Min(100.f,Data()->PetEnergy+10);Tell(TEXT("Kitchen fuel bonus! Extra energy now; 25% less movement energy drain for two minutes."),TEXT("Cozy"));Mode()->Adventure->Save();}
bool UHomeProgression::GearVisible() const{return Gear&&!Gear->IsHidden();}
void UHomeProgression::UpdateGear()
{
    if(!Gear||!Marty())return;Gear->SetActorHiddenInGame(!Enabled()||Level(3)==0||(Data()->CostumeMask&1));if(Gear->IsHidden())return;
    auto* Skin=Marty()->Skin.Get();const auto* Asset=Skin->GetSkinnedAsset();if(!Asset)return;
    const auto& Ref=Asset->GetRefSkeleton();const int32 Index=Ref.FindBoneIndex(TEXT("spine"));if(Index==INDEX_NONE)return;
    FTransform Bind=Ref.GetRefBonePose()[Index];for(int32 Parent=Ref.GetParentIndex(Index);Parent!=INDEX_NONE;Parent=Ref.GetParentIndex(Parent))Bind=Bind*Ref.GetRefBonePose()[Parent];
    const FTransform Saddle(FQuat::Identity,Bind.GetLocation()+FVector(-4,0,3));FTransform Follow=Saddle.GetRelativeTransform(Bind)*Skin->GetBoneTransformByName(TEXT("spine"),EBoneSpaces::WorldSpace);Follow.SetScale3D(FVector::OneVector);Gear->SetActorTransform(Follow);
    for(const auto& C:GearPieces)C->SetMaterial(0,Palette[Level(3)==3?4:Level(3)==2?2:1]);
}
void UHomeProgression::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);Initialize();if(!Ready)return;
    // Signs remain visible behind pause menus, so refresh them before the gameplay guard.
    if(const auto* Guide=GetWorld()->GetGameInstance()->GetSubsystem<UControllerGuide>())
    {
        const int32 Family=int32(Guide->Family());
        if(SignFamily!=Family)
        {
            for(int32 I=0;I<InputSigns.Num();++I)if(InputSigns[I])InputSigns[I]->SetText(FText::FromString(Guide->Format(InputSignSources[I])));
            SignFamily=Family;
        }
    }
    UpdateGear();if(!Playing())return;auto* D=Data();auto* P=Marty();
    if(!Enabled()){CancelActivities();for(const auto& T:ReactionLabels)if(T)T->SetVisibility(false);return;}
    BumpCooldown=FMath::Max(0.f,BumpCooldown-Delta);
    for(float* T:{&D->MealBonusRemaining,&D->RestBonusRemaining,&D->PraiseCooldownRemaining})*T=FMath::Max(0.f,*T-Delta);
    if(Resting){if(P->Life->Activity!=EDogActivity::Lying||FVector::Dist2D(P->GetActorLocation(),Bed)>100)Resting=false;else{RestTime+=Delta;if(Mode()->Adventure->Feature(ECozyFeature::Energy))D->PetEnergy=FMath::Min(100.f,D->PetEnergy+BedEnergyRecovery*Delta);if(Mode()->Adventure->Feature(ECozyFeature::Health))D->PetHealth=FMath::Min(100.f,D->PetHealth+BedHealthRecovery*Delta);if(RestTime>=3&&D->RestBonusRemaining<=0){D->RestBonusRemaining=RestDuration;Tell(TEXT("Well rested! Marty gets 15% less movement energy drain for three minutes."),TEXT("Cozy"));Mode()->Adventure->Save();}}}
    if(Training>=0){TrainingRemaining-=Delta;if(!InAnnex()||TrainingRemaining<=0){Training=-1;Tell(TEXT("Training paused. Start again at a gym mat; no stars were spent."));}else if(Training==2){TrainingSpeed=FMath::Max(TrainingSpeed,float(P->GetVelocity().Size2D()));if(FVector::Dist2D(P->GetActorLocation(),Finish)<75&&FVector::Dist2D(TrainingStart,P->GetActorLocation())>280&&TrainingSpeed>180)FinishTraining();}}
    const auto* PC=GetWorld()->GetFirstPlayerController();FVector View;FRotator Rotation;if(PC)PC->GetPlayerViewPoint(View,Rotation);
    for(int32 I=0;I<2;++I)if(ReactionLabels.IsValidIndex(I)&&ReactionLabels[I]){ReactionTime[I]=FMath::Max(0.f,ReactionTime[I]-Delta);ReactionLabels[I]->SetVisibility(ReactionTime[I]>0&&Mode()->Adventure->Feature(ECozyFeature::WorldLabels)&&GetWorld()->GetGameInstance()->GetSubsystem<UCozyModeHub>()->OverlayVisible(ECozyOverlay::Reactions));if(PC)ReactionLabels[I]->SetWorldRotation((View-ReactionLabels[I]->GetComponentLocation()).Rotation());}
}
