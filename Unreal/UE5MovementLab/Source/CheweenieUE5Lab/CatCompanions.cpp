#include "CatCompanions.h"
#include "HalloweenCostume.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "Neighbourhood.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"

static const TCHAR* CatName(int32 I){return I==0?TEXT("Lakitu"):TEXT("Rio");}
UCatCompanions::UCatCompanions(){PrimaryComponentTick.bCanEverTick=true;}
AMartyCharacter* UCatCompanions::Marty() const{const auto* PC=GetWorld()->GetFirstPlayerController();return PC?Cast<AMartyCharacter>(PC->GetPawn()):nullptr;}
bool UCatCompanions::Enabled() const{const auto* M=Cast<AMartyPlayMode>(GetOwner());return M&&M->Adventure&&M->Adventure->Progress&&M->Adventure->Feature(ECozyFeature::Companions);}
bool UCatCompanions::Following(int32 I) const{const auto* M=Cast<AMartyPlayMode>(GetOwner());return I>=0&&I<2&&M&&M->Adventure->Progress&&(M->Adventure->Progress->CatFollowMask&(1<<I));}
int32 UCatCompanions::CatForStop(FName Id) const{return Id==TEXT("lakitu")?0:Id==TEXT("rio")?1:INDEX_NONE;}
FVector UCatCompanions::Position(int32 I) const{return I>=0&&I<2&&Cats[I].Actor.IsValid()?Cats[I].Actor->GetActorLocation():FVector::ZeroVector;}
bool UCatCompanions::Outdoors(int32 I) const{return I>=0&&I<2&&Cats[I].Outdoors;}
void UCatCompanions::Tell(const FString& Text,int32 Sound){auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure)return;M->Adventure->Message=Text;M->Adventure->HoldMessage(6);if(Sound>=0)M->Adventure->ActionCue(CatName(Sound));}
void UCatCompanions::Initialize()
{
    if(Ready)return;auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure->Progress)return;
    auto* D=M->Adventure->Progress.Get();D->CatFollowMask&=3;D->LakituBond=FMath::Clamp(D->LakituBond,0,10);D->RioBond=FMath::Clamp(D->RioBond,0,10);
    for(TActorIterator<AStaticMeshActor> It(GetWorld());It;++It){const UStaticMesh* PetMesh=It->GetStaticMeshComponent()->GetStaticMesh();if(!PetMesh)continue;
        for(int32 I=0;I<2;++I)if(PetMesh->GetPathName()==FString::Printf(TEXT("/Game/VisualLab/Pets/%s.%s"),CatName(I),CatName(I))){auto& C=Cats[I];C.Actor=*It;C.Home=It->GetActorTransform();C.Position=It->GetActorLocation();It->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);It->SetActorEnableCollision(false);}}
    for(auto& Cat:Cats)if(Cat.Actor.IsValid()&&!Cat.Actor->FindComponentByClass<UHalloweenCostume>()){auto* Outfit=NewObject<UHalloweenCostume>(Cat.Actor.Get());Outfit->RegisterComponent();}
    Ready=Cats[0].Actor.IsValid()&&Cats[1].Actor.IsValid();
}
FString UCatCompanions::State(int32 I) const{return !Enabled()?TEXT("OFF"):Cats[I].Happy>0?TEXT("Purring"):Cats[I].Waiting?TEXT("Waiting"):Following(I)?TEXT("Following"):TEXT("Staying");}
FString UCatCompanions::Status() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure->Progress)return TEXT("Getting the crew ready...");const auto* D=M->Adventure->Progress.Get();
    return FString::Printf(TEXT("Bond: Lakitu %d/10 | Rio %d/10%s"),D->LakituBond,D->RioBond,D->LakituBond>=3&&D->RioBond>=3?TEXT(" | Best buddies!"):TEXT(""));
}
bool UCatCompanions::PlaceNear(int32 I,bool Outside)
{
    auto* P=Marty();auto& C=Cats[I];if(!P||!C.Actor.IsValid())return false;
    FCollisionQueryParams Q(SCENE_QUERY_STAT(CatRecall),false,P);for(auto& Cat:Cats)if(Cat.Actor.IsValid())Q.AddIgnoredActor(Cat.Actor.Get());
    const FVector Offsets[]={FVector(-100,I==0?-45:45,0),FVector(0,I==0?-120:120,0),FVector(110,I==0?-45:45,0)};
    for(FVector Offset:Offsets){const FVector Trial=P->GetActorLocation()+P->GetActorRotation().RotateVector(Offset);FHitResult Ground;
        if(!GetWorld()->LineTraceSingleByChannel(Ground,Trial+FVector(0,0,80),Trial-FVector(0,0,180),ECC_WorldStatic,Q)||Ground.ImpactNormal.Z<.7f)continue;
        const FVector Spot(Ground.ImpactPoint.X,Ground.ImpactPoint.Y,Ground.ImpactPoint.Z+C.Home.GetLocation().Z);
        if(GetWorld()->OverlapBlockingTestByChannel(Spot+FVector(0,0,22),FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeSphere(14),Q))continue;
        C.Position=Spot;C.Speed=0;C.Waiting=false;C.Outdoors=Outside;C.Actor->SetActorLocationAndRotation(Spot,FRotator(0,P->GetActorRotation().Yaw,0));return true;}
    C.Waiting=true;return false;
}
void UCatCompanions::OnPlayerTravel(bool Outside){Initialize();if(!Enabled()||!Ready)return;for(int32 I=0;I<2;++I)if(Following(I))PlaceNear(I,Outside);}
void UCatCompanions::ApplyFeature()
{
    Initialize();if(!Ready)return;for(auto& C:Cats){C.Actor->SetActorTransform(C.Home);C.Position=C.Home.GetLocation();C.Speed=C.Happy=0;C.Waiting=C.Outdoors=false;}
    const auto* M=Cast<AMartyPlayMode>(GetOwner());if(Enabled()&&M&&M->Neighbourhood)OnPlayerTravel(M->Neighbourhood->Outside);
}
void UCatCompanions::ToggleFollow(int32 I)
{
    Initialize();auto* M=Cast<AMartyPlayMode>(GetOwner());if(!Enabled()||!Ready||I<0||I>1){Tell(TEXT("Enable companion behaviour in Controls > Pet features."));return;}
    M->Adventure->Progress->CatFollowMask^=1<<I;Cats[I].Speed=0;Cats[I].Waiting=false;
    if(Following(I)&&Cats[I].Outdoors!=M->Neighbourhood->Outside)PlaceNear(I,M->Neighbourhood->Outside);
    Tell(FString(CatName(I))+(Following(I)?TEXT(" is joining Marty's little patrol."):TEXT(" will stay here. Call the crew with K / D-pad Down.")),I);M->Adventure->Save();
}
void UCatCompanions::CallBoth(){if(!Enabled()){Tell(TEXT("Enable companion behaviour in Controls > Pet features."));return;}auto* M=Cast<AMartyPlayMode>(GetOwner());M->Adventure->Progress->CatFollowMask=3;OnPlayerTravel(M->Neighbourhood->Outside);Tell(TEXT("Crew, assemble! Lakitu and Rio join Marty."));M->Adventure->Save();}
void UCatCompanions::SendHome(){auto* M=Cast<AMartyPlayMode>(GetOwner());if(!M||!M->Adventure->Progress)return;M->Adventure->Progress->CatFollowMask=0;ApplyFeature();Tell(TEXT("The cats return to their cozy headquarters spots."));M->Adventure->Save();}
bool UCatCompanions::Pet(int32 I)
{
    Initialize();auto* M=Cast<AMartyPlayMode>(GetOwner());auto* P=Marty();if(!Enabled()||!Ready||!P||I<0||I>1)return false;
    if(Cats[I].Outdoors!=M->Neighbourhood->Outside||FVector::Dist2D(P->GetActorLocation(),Position(I))>130){Tell(FString(TEXT("Come closer to "))+CatName(I)+TEXT(" for a friendly head bump."));return false;}
    if(Cats[I].PetCooldown>0){Tell(FString(CatName(I))+TEXT(" is still enjoying that head bump."));return true;}
    auto* D=M->Adventure->Progress.Get();int32& Bond=I==0?D->LakituBond:D->RioBond;Bond=FMath::Min(10,Bond+1);Cats[I].PetCooldown=6;Cats[I].Happy=5;
    Tell(D->LakituBond>=3&&D->RioBond>=3?TEXT("Leo Ellis reports: Best Buddies badge earned. Extremely official cat business."):FString(CatName(I))+TEXT(" leans in for a head bump. Friendship grows!"),I);M->Adventure->Save();return true;
}
bool UCatCompanions::PetNearby(){const auto* M=Cast<AMartyPlayMode>(GetOwner());auto* P=Marty();if(!Enabled()||!P||!M)return false;for(int32 I=0;I<2;++I)if(Cats[I].Outdoors==M->Neighbourhood->Outside&&FVector::Dist2D(P->GetActorLocation(),Position(I))<110)return Pet(I);return false;}
FString UCatCompanions::Prompt() const
{
    const auto* M=Cast<AMartyPlayMode>(GetOwner());auto* P=Marty();if(!Enabled()||!P||!M)return FString();for(int32 I=0;I<2;++I)if(Cats[I].Outdoors==M->Neighbourhood->Outside&&FVector::Dist2D(P->GetActorLocation(),Position(I))<110)return FString(TEXT("E / X-Square: head bump with "))+CatName(I)+TEXT(" | K / D-pad Down: crew commands");return FString();
}
void UCatCompanions::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);Initialize();auto* M=Cast<AMartyPlayMode>(GetOwner());auto* P=Marty();if(Ready&&M&&M->Adventure->Progress)for(int32 I=0;I<2;++I)if(auto* Outfit=Cats[I].Actor->FindComponentByClass<UHalloweenCostume>())Outfit->SetOutfit(Cats[I].Actor->GetStaticMeshComponent(),I+1,(M->Adventure->Progress->CostumeMask&(1<<(I+1)))!=0);
    if(!Ready||!Enabled()||!P||M->Adventure->Screen!=ECozyScreen::Playing)return;
    const float Dt=FMath::Min(Delta,.05f);
    for(int32 I=0;I<2;++I){auto& C=Cats[I];C.Phase+=Dt;C.PetCooldown=FMath::Max(0.f,C.PetCooldown-Dt);C.Happy=FMath::Max(0.f,C.Happy-Dt);
        FVector Direction=FVector::ZeroVector;float TargetSpeed=0;
        if(Following(I)&&C.Outdoors==M->Neighbourhood->Outside){const FVector Goal=P->GetActorLocation()+P->GetActorRotation().RotateVector(FVector(-125,I==0?-55:55,0));Direction=Goal-C.Position;Direction.Z=0;const float Distance=Direction.Size();if(Distance>35)TargetSpeed=FMath::Min(Distance>350?CatchupSpeed:I==0?LakituSpeed:RioSpeed,(Distance-25)*1.5f);}
        C.Speed=FMath::FInterpConstantTo(C.Speed,TargetSpeed,Dt,Acceleration);C.Waiting=Following(I)&&C.Outdoors!=M->Neighbourhood->Outside;
        if(C.Speed>1&&!Direction.IsNearlyZero()){
            const FVector Step=Direction.GetSafeNormal()*FMath::Min(C.Speed*Dt,Direction.Size());FVector Candidate=C.Position+Step;FCollisionQueryParams Q(SCENE_QUERY_STAT(CatWalk),false,P);for(auto& Cat:Cats)if(Cat.Actor.IsValid())Q.AddIgnoredActor(Cat.Actor.Get());FHitResult Hit;
            const FVector Lift(0,0,22);if(GetWorld()->SweepSingleByChannel(Hit,C.Position+Lift,Candidate+Lift,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeSphere(14),Q)){
                Candidate=C.Position+FVector::VectorPlaneProject(Step,Hit.ImpactNormal);if(GetWorld()->SweepSingleByChannel(Hit,C.Position+Lift,Candidate+Lift,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeSphere(14),Q)){Candidate=C.Position;C.Waiting=true;}}
            FHitResult Ground;if(GetWorld()->LineTraceSingleByChannel(Ground,Candidate+FVector(0,0,65),Candidate-FVector(0,0,100),ECC_WorldStatic,Q)&&Ground.ImpactNormal.Z>.7f&&FMath::Abs(Ground.ImpactPoint.Z+C.Home.GetLocation().Z-C.Position.Z)<25){Candidate.Z=Ground.ImpactPoint.Z+C.Home.GetLocation().Z;if(FVector::Dist2D(C.Position,Candidate)<.01f)C.Waiting=true;C.Position=Candidate;}else C.Waiting=true;
        }
        const FVector Look=C.Speed>1&&!C.Waiting&&!Direction.IsNearlyZero()?Direction:P->GetActorLocation()-C.Position;float Yaw=C.Actor->GetActorRotation().Yaw;
        if(C.Outdoors==M->Neighbourhood->Outside&&(C.Speed>1||Look.Size2D()<220))Yaw=FMath::FixedTurn(Yaw,Look.Rotation().Yaw,TurnSpeed*Dt);
        const float Bob=C.Speed>1&&!C.Waiting?.35f*FMath::Sin(C.Phase*12):.12f*FMath::Sin(C.Phase*2);C.Actor->SetActorLocationAndRotation(C.Position+FVector(0,0,Bob),FRotator(0,Yaw,0));
    }
}
