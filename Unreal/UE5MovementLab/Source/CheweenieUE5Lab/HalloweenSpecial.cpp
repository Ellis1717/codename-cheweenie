#include "HalloweenSpecial.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "MartyCharacter.h"
#include "Neighbourhood.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

static AMartyPlayMode* FestivalMode(const UActorComponent* C){return Cast<AMartyPlayMode>(C->GetOwner());}
static const TCHAR* PorchNames[]={TEXT("Pumpkin Post"),TEXT("Sock Goblin Stop"),TEXT("Captain Couch"),TEXT("Purrcorn Porch"),TEXT("Biscuit Bureau"),TEXT("Moonlight Mailbox"),TEXT("Blanket Inspectors"),TEXT("Cozy Corner")};
UHalloweenSpecial::UHalloweenSpecial(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.1f;}
bool UHalloweenSpecial::Active() const{const auto* M=FestivalMode(this);return M&&M->Adventure&&M->Adventure->Progress&&M->Adventure->Progress->HalloweenActive;}
int32 UHalloweenSpecial::StampCount() const{const auto* M=FestivalMode(this);if(!M||!M->Adventure->Progress)return 0;int32 N=0;for(int32 I=0;I<8;++I)N+=(M->Adventure->Progress->HalloweenStampMask>>I)&1;return N;}
FVector UHalloweenSpecial::Clubhouse() const{const auto* M=FestivalMode(this);return M&&M->Neighbourhood?M->Neighbourhood->Entrance+FVector(0,-300,0):FVector::ZeroVector;}
void UHalloweenSpecial::Prepare()
{
    auto* M=FestivalMode(this);if(!M||!M->Adventure->Progress||!M->Neighbourhood||M->Neighbourhood->Dogs.Num()!=8||Porches.Num())return;
    auto* D=M->Adventure->Progress.Get();D->HalloweenStampMask&=255;
    if(D->HalloweenRoundComplete&&D->HalloweenStampMask!=255)D->HalloweenRoundComplete=false;
    for(int32 I=0;I<8;++I){const float Side=I%2==0?-1.f:1.f;Porches.Add({M->Neighbourhood->Dogs[I].Position+FVector(0,Side*260,28),PorchNames[I]});}
    if(Active())BuildDecorations();
}
int32 UHalloweenSpecial::NextPorch() const
{
    const auto* M=FestivalMode(this);const auto* P=UGameplayStatics::GetPlayerPawn(this,0);if(!M||!M->Adventure->Progress||!P)return INDEX_NONE;
    double Best=DBL_MAX;int32 Pick=INDEX_NONE;
    for(int32 I=0;I<Porches.Num();++I)if(!(M->Adventure->Progress->HalloweenStampMask&(1<<I))){const double D=FVector::DistSquared2D(P->GetActorLocation(),Porches[I].Position);if(D<Best){Best=D;Pick=I;}}
    return Pick;
}
FVector UHalloweenSpecial::GoalPosition() const
{
    const int32 I=NextPorch();if(Porches.IsValidIndex(I))return Porches[I].Position;
    const auto* M=FestivalMode(this);return M&&M->Neighbourhood->Outside?Clubhouse():FVector(-200,-200,28);
}
FString UHalloweenSpecial::Objective() const
{
    const auto* M=FestivalMode(this);if(!Active())return FString();
    if(M->Adventure->Progress->HalloweenRoundComplete)return TEXT("Pumpkin Pal earned! Enjoy the lantern walk or replay from the Halloween menu.");
    const int32 I=NextPorch();return I>=0?FString::Printf(TEXT("Bork or Treat: %d / 8 stamps | Next: %s"),StampCount(),*Porches[I].Name):TEXT("8 / 8 stamps! Bring your paw-pass back to headquarters for the cozy finale.");
}
FString UHalloweenSpecial::Prompt() const
{
    const auto* M=FestivalMode(this);const auto* P=UGameplayStatics::GetPlayerPawn(this,0);if(!Active()||!M||!P||M->Adventure->Progress->HalloweenRoundComplete)return FString();
    if(StampCount()==8&&FVector::Dist2D(P->GetActorLocation(),GoalPosition())<145)return TEXT("E / X-Square: celebrate at the pumpkin clubhouse");
    if(!M->Neighbourhood->Outside)return FString();
    for(int32 I=0;I<Porches.Num();++I)if(FVector::Dist2D(P->GetActorLocation(),Porches[I].Position)<145)
        return (M->Adventure->Progress->HalloweenStampMask&(1<<I))?TEXT("E / X-Square: say happy Halloween again"):TEXT("E / X-Square: Bork or Treat! ")+Porches[I].Name;
    return FString();
}
bool UHalloweenSpecial::Interact(AMartyCharacter* Pawn)
{
    auto* M=FestivalMode(this);if(!Pawn||Prompt().IsEmpty()||!M)return false;auto* S=M->Adventure.Get();auto* D=S->Progress.Get();
    if(StampCount()==8&&FVector::Dist2D(Pawn->GetActorLocation(),GoalPosition())<145)
    {
        D->HalloweenBadge=true;D->HalloweenRoundComplete=true;
        S->Message=TEXT("Leo Ellis, Blanket Boss: Pumpkin Pal badge awarded! Fox Ellis confirms: eight porches, zero actual ghosts. Cheeween Cozy time!");
        S->ActionCue(TEXT("Cozy"));
    }
    else
    {
        for(int32 I=0;I<Porches.Num();++I)if(FVector::Dist2D(Pawn->GetActorLocation(),Porches[I].Position)<145)
        {
            if(D->HalloweenStampMask&(1<<I)){S->Message=TEXT("Already stamped! This pumpkin insists it used to be a turnip.");S->HoldMessage(6);return true;}
            static const TCHAR* Greetings[]={TEXT("Fox Ellis, Chief Sniff Inspector: Costume approved. Dog still suspiciously dog-shaped."),TEXT("Keeterman: I came for treats. I stayed for the doorbell."),TEXT("Rio: One sticker. Three compliments. Acceptable service."),TEXT("Pumpkin committee: Bork softly. The pumpkins are napping."),TEXT("Marty Party's snack audit: this leaf is definitely not a snack."),TEXT("Fox Ellis reports: Zero ghosts. One extremely dramatic bedsheet."),TEXT("Leo Ellis, Blanket Boss: All visitors must pass the comfy test."),TEXT("Best costume? A tie. The cat has already appealed.")};
            D->HalloweenStampMask|=1<<I;S->Message=FString(Greetings[I])+TEXT("  Paw-print sticker collected!");S->ActionCue(TEXT("Discovery"));break;
        }
    }
    S->HoldMessage(9);S->Save();Refresh();return true;
}
void UHalloweenSpecial::Refresh()
{
    Prepare();auto* M=FestivalMode(this);if(!M)return;
    if(Active()&&!Decor)BuildDecorations();LastVisibility=-1;
    if(M->Navigation)M->Navigation->RefreshRoute();
    M->Neighbourhood->SetOutdoorLighting(M->Neighbourhood->Outside);
}
void UHalloweenSpecial::StartTrail()
{
    auto* M=FestivalMode(this);if(!M||!M->Adventure->Progress)return;
    M->Adventure->Progress->HalloweenActive=true;M->Adventure->Progress->NavTarget=0;
    M->Adventure->Start(false);if(auto* P=Cast<AMartyCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))M->Neighbourhood->GoOutside(P);
    M->Adventure->Message=TEXT("BORK OR TREAT! Visit eight lantern porches for paw-print stickers, then bring your card home. No timer. Costumes are optional.");
    M->Adventure->HoldMessage(10);M->Adventure->Save();Refresh();
}
void UHalloweenSpecial::Toggle(){auto* M=FestivalMode(this);if(!M||!M->Adventure->Progress)return;M->Adventure->Progress->HalloweenActive=!Active();M->Adventure->Save();Refresh();}
void UHalloweenSpecial::Replay(){auto* M=FestivalMode(this);if(!M||!M->Adventure->Progress)return;M->Adventure->Progress->HalloweenStampMask=0;M->Adventure->Progress->HalloweenRoundComplete=false;StartTrail();}
void UHalloweenSpecial::Instance(const TCHAR* Mesh,const TCHAR* Material,FVector P,FVector Size,FRotator R)
{
    const FString Key=FString(Mesh)+Material;
    if(!Batches.Contains(Key))
    {
        auto* C=NewObject<UInstancedStaticMeshComponent>(Decor);C->SetMobility(EComponentMobility::Movable);C->SetupAttachment(Decor->GetRootComponent());
        C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,*FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"),Mesh,Mesh)));
        C->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Adventure/Halloween/%s.%s"),Material,Material)));
        C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetGenerateOverlapEvents(false);C->SetCanEverAffectNavigation(false);C->RegisterComponent();Batches.Add(Key,C);
    }
    Batches[Key]->AddInstance(FTransform(R,P,Size/100),true);
}
void UHalloweenSpecial::BuildDecorations()
{
    if(Decor||Porches.Num()!=8)return;
    Decor=GetWorld()->SpawnActor<AActor>();Decor->Tags.Add(TEXT("HalloweenSpecialDecor"));
    auto* Root=NewObject<USceneComponent>(Decor);Decor->SetRootComponent(Root);Root->RegisterComponent();
    for(int32 I=0;I<9;++I)
    {
        const float Front=I<8?(I%2==0?1.f:-1.f):-1.f;
        const FVector P=(I<8?Porches[I].Position:Clubhouse())-FVector(0,0,28);
        for(float Side:{-1.f,1.f})
        {
            const FVector Pumpkin=P+FVector(Side*105,0,24);
            Instance(TEXT("Sphere"),TEXT("Orange"),Pumpkin,FVector(43,40,38));
            Instance(TEXT("Cylinder"),TEXT("Green"),Pumpkin+FVector(0,0,24),FVector(6,6,13));
            for(float Eye:{-1.f,1.f})Instance(TEXT("Cube"),TEXT("FestivalGlow"),Pumpkin+FVector(Eye*9,Front*19,5),FVector(6,3,7));
            Instance(TEXT("Sphere"),TEXT("FestivalGlow"),Pumpkin+FVector(0,Front*19,-6),FVector(17,3,5));
            Instance(TEXT("Cube"),TEXT("Black"),P+FVector(Side*150,0,29),FVector(18,18,46));
            Instance(TEXT("Sphere"),TEXT("FestivalGlow"),P+FVector(Side*150,Front*10,31),FVector(12,8,29));
        }
        // A freestanding purple porch pennant leaves the approach clear.
        Instance(TEXT("Cube"),TEXT("Green"),P+FVector(-110,30,75),FVector(5,5,150));
        Instance(TEXT("Cube"),TEXT("FestivalPlum"),P+FVector(-110,30,130),FVector(70,6,38));
        auto* Text=NewObject<UTextRenderComponent>(Decor);Text->SetupAttachment(Root);Text->RegisterComponent();Text->SetWorldLocation(P+FVector(0,0,105));Text->SetWorldSize(20);Text->SetHorizontalAlignment(EHTA_Center);Text->SetCollisionEnabled(ECollisionEnabled::NoCollision);Text->SetCanEverAffectNavigation(false);Labels.Add(Text);
        auto* Light=NewObject<UPointLightComponent>(Decor);Light->SetupAttachment(Root);Light->RegisterComponent();Light->SetWorldLocation(P+FVector(0,0,70));Light->SetIntensity(220);Light->SetAttenuationRadius(260);Light->SetLightColor(FLinearColor(1,.42f,.10f));Light->SetCastShadows(false);Lights.Add(Light);
    }
}
bool UHalloweenSpecial::DecorationsVisible() const{return Decor&&!Decor->IsHidden();}
bool UHalloweenSpecial::DecorationsCollisionFree() const{for(const auto& Pair:Batches)if(Pair.Value->GetCollisionEnabled()!=ECollisionEnabled::NoCollision||Pair.Value->CanEverAffectNavigation())return false;return true;}
void UHalloweenSpecial::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);Prepare();if(Active()&&!Decor)BuildDecorations();if(!Decor)return;
    const int32 Visible=Active()?1:0;if(Visible!=LastVisibility){LastVisibility=Visible;Decor->SetActorHiddenInGame(!Visible);for(auto& L:Lights)L->SetVisibility(Visible!=0);}
    if(!Visible)return;
    const auto* M=FestivalMode(this);const auto* PC=GetWorld()->GetFirstPlayerController();if(!M||!PC||!PC->PlayerCameraManager)return;
    const FVector Camera=PC->PlayerCameraManager->GetCameraLocation();
    for(int32 I=0;I<Labels.Num();++I)
    {
        auto* L=Labels[I].Get();L->SetVisibility(M->Adventure->Feature(ECozyFeature::WorldLabels)&&FVector::Dist2D(Camera,L->GetComponentLocation())<1800);
        L->SetWorldRotation((Camera-L->GetComponentLocation()).Rotation());
        L->SetText(FText::FromString(I==8?TEXT("PUMPKIN CLUBHOUSE"):((M->Adventure->Progress->HalloweenStampMask&(1<<I))?TEXT("STAMPED! "):TEXT("BORK OR TREAT\n"))+Porches[I].Name));
        L->SetTextRenderColor((I<8&&(M->Adventure->Progress->HalloweenStampMask&(1<<I)))?FColor(150,240,190):FColor(255,204,115));
    }
}
