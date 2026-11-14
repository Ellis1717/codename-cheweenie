#include "AnimalGait.h"
#include "MartyGait.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
UAnimalGait::UAnimalGait(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickGroup=TG_PostPhysics;}
bool UAnimalGait::Initialize(const FString& Profile,bool Cat,USceneComponent* Parent,FTransform Local)
{
    if(Ready&&BoundProfile==Profile)return true;
    Ready=false;auto* Mesh=LoadObject<USkeletalMesh>(nullptr,*FString::Printf(TEXT("/Game/Adventure/CompanionRigs/%s_Rig.%s_Rig"),*Profile,*Profile));if(!Mesh||!Parent)return false;
    if(!Skin)
    {
        Skin=NewObject<UPoseableMeshComponent>(GetOwner());Skin->SetupAttachment(Parent);Skin->SetMobility(EComponentMobility::Movable);
        Skin->SetCollisionEnabled(ECollisionEnabled::NoCollision);Skin->SetCanEverAffectNavigation(false);
        Skin->VisibilityBasedAnimTickOption=EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;Skin->RegisterComponent();
    }
    Skin->SetRelativeTransform(Local);Skin->SetSkinnedAssetAndUpdate(Mesh);Feline=Cat;BoundProfile=Profile;Rest.Empty();Legs.Empty();
    const auto& Ref=Mesh->GetRefSkeleton();
    for(int32 I=0;I<Ref.GetNum();++I)
    {
        FTransform B=Ref.GetRefBonePose()[I];for(int32 P=Ref.GetParentIndex(I);P!=INDEX_NONE;P=Ref.GetParentIndex(P))B=B*Ref.GetRefBonePose()[P];Rest.Add(Ref.GetBoneName(I),B);
    }
    for(const TCHAR* N:{TEXT("pelvis"),TEXT("spine"),TEXT("chest"),TEXT("neck"),TEXT("head"),TEXT("tail"),TEXT("tail_mid"),TEXT("tail_tip"),TEXT("ear_l"),TEXT("ear_r")})if(!Rest.Contains(N))return false;
    for(int32 I=0;I<4;++I)
    {
        FString Key=FString(I<2?TEXT("front"):TEXT("hind"))+(I%2==0?TEXT("_l"):TEXT("_r"));FAnimalLeg L;
        L.Upper=FName(Key+TEXT("_upper"));L.Lower=FName(Key+TEXT("_lower"));L.Hock=FName(Key+TEXT("_hock"));L.Paw=FName(Key+TEXT("_paw"));
        for(FName N:{L.Upper,L.Lower,L.Hock,L.Paw})if(!Rest.Contains(N))return false;
        L.Hip=Rest[L.Upper].GetLocation();L.Knee=Rest[L.Lower].GetLocation();L.Ankle=Rest[L.Hock].GetLocation();L.Toe=Rest[L.Paw].GetLocation();Legs.Add(L);
    }
    Ready=true;Time=Cycle=Speed=0;ResetContacts();return true;
}
FVector UAnimalGait::Ground(FVector Point) const
{
    FHitResult H;FCollisionQueryParams Q(SCENE_QUERY_STAT(AnimalPaw),false,GetOwner());
    if(GetWorld()->LineTraceSingleByObjectType(H,Point+FVector(0,0,35),Point-FVector(0,0,65),FCollisionObjectQueryParams(ECC_WorldStatic),Q)&&H.ImpactNormal.Z>.7f)Point.Z=H.ImpactPoint.Z+1.8f*Skin->GetComponentScale().Z;
    return Point;
}
void UAnimalGait::ResetContacts()
{
    if(!Ready)return;Last=Skin->GetComponentLocation();LastYaw=Skin->GetComponentRotation().Yaw;TurnBalance=0;Speed=0;++ContactResets;
    for(auto& L:Legs){L.Planted=L.PrintValid=L.PreviousPrintValid=L.Swinging=L.RegisteredSwing=false;FVector Toe=L.Toe;if(Feline)Toe.Y*=.84f;L.Contact=Ground(Skin->GetComponentTransform().TransformPosition(Toe));L.LiftOff=L.LastPlant=L.PreviousPlant=L.SwingTarget=L.Contact;}
}
void UAnimalGait::Aim(FName Bone,FVector Start,FVector RestEnd,FVector End)
{
    FTransform T=Rest[Bone];T.SetLocation(Start);T.SetRotation(FQuat::FindBetweenNormals((RestEnd-Rest[Bone].GetLocation()).GetSafeNormal(),(End-Start).GetSafeNormal())*T.GetRotation());Skin->SetBoneTransformByName(Bone,T,EBoneSpaces::ComponentSpace);
}
void UAnimalGait::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);if(!Ready||!Skin->IsVisible())return;
    auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();if(M&&M->Adventure&&M->Adventure->Screen!=ECozyScreen::Playing)return;
    const float Dt=FMath::Min(Delta,.05f);if(Dt<=0)return;
    FVector Position=Skin->GetComponentLocation();float Distance=FVector::Dist2D(Position,Last);
    const float Yaw=Skin->GetComponentRotation().Yaw;const float Turn=FRotator::NormalizeAxis(Yaw-LastYaw);
    if(Distance>100||FMath::Abs(Turn)>65){ResetContacts();Distance=0;}
    else TurnBalance=FMath::FInterpTo(TurnBalance,Feline?FMath::Clamp(Turn/(Dt*180.f),-1.f,1.f):0.f,Dt,5.f);
    LastYaw=Yaw;Last=Position;
    Speed=FMath::FInterpTo(Speed,Distance/Dt,Dt,10);Time+=Dt;Travel+=Distance;
    const float Scale=Skin->GetComponentScale().X,LocalSpeed=Speed/FMath::Max(Scale,.1f);
    const float Fast=FMath::Clamp((LocalSpeed-115)/75,0.f,1.f),Moving=FMath::Clamp(LocalSpeed/20,0.f,1.f);
    const float Bound=Feline?FMath::Clamp((LocalSpeed-200.f)/50.f,0.f,1.f):0.f;
    const float Stride=(Feline?30.f:39.f)*Scale*(1+.3f*Fast+.12f*Bound);
    Cycle=FMath::Fmod(Cycle+Distance/FMath::Max(Stride,1.f),1.f);
    const auto* Character=Cast<ACharacter>(GetOwner());const bool Air=(Character&&Character->GetCharacterMovement()->IsFalling())||GetOwner()->ActorHasTag(TEXT("AnimalAirborne"));
    if(!Air&&WasAir)ResetContacts();
    if(Air)for(auto& L:Legs){L.PrintValid=L.PreviousPrintValid=false;}
    WasAir=Air;
    const float Wave=Cycle*UE_TWO_PI;
    const float Bob=Moving*FMath::Sin(Wave*2)*(Feline?.24f+Bound*.45f:.65f)+(1-Moving)*FMath::Sin(Time*2)*.13f-(Feline?.65f*Moving:0.f);
    const float Sway=Moving*FMath::Sin(Wave)*(Feline?.012f:.018f);
    BodyMotion=Bob;Skin->ResetBoneTransformByName(TEXT("root"));
    for(const FName N:{FName(TEXT("pelvis")),FName(TEXT("spine")),FName(TEXT("chest")),FName(TEXT("neck")),FName(TEXT("head"))})
    {
        FTransform T=Rest[N];const float Weight=N==TEXT("pelvis")?-1.f:N==TEXT("spine")?0.f:1.f;
        const float Flex=Feline?FMath::Sin(Wave+Weight*.5f)*Moving*(.018f+Fast*.035f+Bound*.12f):FMath::Sin(Wave*2)*Moving*.018f;
        T.AddToTranslation(FVector(0,FMath::Sin(Wave)*Moving*Weight*.4f,Bob+Weight*FMath::Sin(Wave)*Moving*.35f));
        T.SetRotation(FQuat(FVector::ForwardVector,Sway*Weight)*FQuat(FVector::RightVector,Flex+(Sniff&&N==TEXT("head")?.28f:0))*T.GetRotation());
        if(N==TEXT("head"))
        {
            if(Feline){T.AddToTranslation(FVector(0,0,-Bob*.85f-FMath::Sin(Wave)*Moving*.30f));T.SetRotation(FQuat(FVector::RightVector,-Flex*.8f)*T.GetRotation());}
            T.AddToTranslation(FVector(0,0,Happy?.6f*FMath::Sin(Time*5):0));
        }
        Skin->SetBoneTransformByName(N,T,EBoneSpaces::ComponentSpace);
    }
    const float Walk[]={0,.5f,.75f,.25f},CatWalk[]={0,.5f,.18f,.68f},Trot[]={0,.5f,.5f,0},CatBound[]={0,.1f,.52f,.62f};
    const FTransform World=Skin->GetComponentTransform();PlantedPaws=0;MaxJointBend=0;
    for(int32 I=0;I<Legs.Num();++I)
    {
        auto& L=Legs[I];const float Phase=FMath::Fmod(Cycle+FMath::Lerp(FMath::Lerp(Feline?CatWalk[I]:Walk[I],Trot[I],Fast),Feline?CatBound[I]:Trot[I],Bound),1.f);
        const float Stance=FMath::Lerp(FMath::Lerp(Feline?.62f:.69f,.52f,Fast),.48f,Bound);const bool Plant=Phase<Stance||Moving<.03f;
        FVector RestToe=L.Toe;if(Feline)RestToe.Y*=.84f;
        const FVector RestWorld=World.TransformPosition(RestToe);
        FVector Target=Ground(RestWorld+World.GetRotation().GetForwardVector()*Stride*(Feline?.48f:.20f));
        if(Air){L.Planted=L.Swinging=L.RegisteredSwing=false;L.Contact=World.TransformPosition(RestToe+FVector(I<2?2:-2,0,9));}
        else if(Plant)
        {
            if(!L.Planted)
            {
                L.Contact=Ground(Feline&&L.Swinging?L.SwingTarget:L.Contact);
                L.PreviousPlant=L.LastPlant;L.PreviousPrintValid=L.PrintValid;L.LastPlant=L.Contact;L.PrintValid=true;L.Swinging=false;
            }
            if(FVector::Dist2D(L.Contact,RestWorld)>Stride*.65f){L.Contact=Ground(RestWorld);L.RegisteredSwing=false;if(Feline&&I<2){L.LastPlant=L.Contact;L.PrintValid=true;L.PreviousPrintValid=false;}}
            L.Planted=true;L.LiftOff=L.Contact;++PlantedPaws;
        }
        else
        {
            if(L.Planted||!L.Swinging)
            {
                L.LiftOff=L.Contact;L.Planted=false;L.Swinging=true;L.RegisteredSwing=false;
                // Follow an actual same-side front print, never a moving swing paw.
                if(Feline&&I>=2&&Bound<.1f&&Fast<.25f)
                {
                    const auto& Front=Legs[I-2];float Best=Stride*.55f;FVector Pick=Target;bool Found=false;
                    const FVector FutureHip=World.TransformPosition(L.Hip+FVector(0,0,Bob))+World.GetRotation().GetForwardVector()*Stride*(1-Stance);
                    const float Reach=(FVector::Dist(L.Hip,L.Knee)+FVector::Dist(L.Knee,L.Ankle)+FVector::Dist(L.Ankle,L.Toe))*Scale;
                    for(int32 Print=0;Print<2;++Print)
                    {
                        if(!(Print==0?Front.PrintValid:Front.PreviousPrintValid))continue;
                        const FVector Candidate=(Print==0?Front.LastPlant:Front.PreviousPlant)-World.GetRotation().GetForwardVector()*2.f*Scale;
                        const float Error=FVector::Dist2D(Candidate,Target);
                        if(Error<Best&&FVector::Dist(Candidate,FutureHip)<Reach){Best=Error;Pick=Candidate;Found=true;}
                    }
                    if(Found){Target=Ground(Pick);L.RegisteredSwing=true;++RegisteredHindSteps;}
                }
                L.SwingTarget=Target;
            }
            float T=(Phase-Stance)/(1-Stance);const float Smooth=Feline?T*T*T*(10+T*(-15+6*T)):T*T*(3-2*T);
            const float Arc=Feline?FMath::Pow(FMath::Max(0.f,FMath::Sin(T*UE_PI)),1.4f):FMath::Sin(T*UE_PI);
            L.Contact=FMath::Lerp(L.LiftOff,Feline?L.SwingTarget:Target,Smooth)+FVector(0,0,Arc*(Feline?3.6f:6.f)*Scale*Moving);
        }
        const FTransform Parent=Skin->GetBoneTransformByName(I<2?TEXT("chest"):TEXT("pelvis"),EBoneSpaces::ComponentSpace);
        FVector Hip=Parent.TransformPosition(Rest[I<2?TEXT("chest"):TEXT("pelvis")].InverseTransformPosition(L.Hip));
        if(Feline&&I<2)Hip+=FVector(FMath::Sin(Phase*UE_TWO_PI)*1.1f*Moving,0,FMath::Cos(Phase*UE_TWO_PI)*.65f*Moving);
        FVector Toe=World.InverseTransformPosition(L.Contact),Ankle=Toe+L.Ankle-L.Toe;
        const float Upper=FVector::Dist(L.Hip,L.Knee),Lower=FVector::Dist(L.Knee,L.Ankle);
        Ankle=Hip+(Ankle-Hip).GetClampedToMaxSize(Upper+Lower-.04f);
        const FVector Knee=UMartyGait::SolveKnee(Hip,Ankle,Upper,Lower,I<2?-1.f:1.f);
        Aim(L.Upper,Hip,L.Knee,Knee);Aim(L.Lower,Knee,L.Ankle,Ankle);Aim(L.Hock,Ankle,L.Toe,Toe);
        FTransform Paw=Rest[L.Paw];Paw.SetLocation(Toe);Skin->SetBoneTransformByName(L.Paw,Paw,EBoneSpaces::ComponentSpace);
        MaxJointBend=FMath::Max(MaxJointBend,FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct((Knee-Hip).GetSafeNormal(),(Ankle-Knee).GetSafeNormal()),-1.f,1.f))));
    }
    for(const FName N:{FName(TEXT("tail")),FName(TEXT("tail_mid")),FName(TEXT("tail_tip")),FName(TEXT("ear_l")),FName(TEXT("ear_r"))})
    {
        const bool Ear=N==TEXT("ear_l")||N==TEXT("ear_r");FName Parent=Ear?TEXT("head"):N==TEXT("tail")?TEXT("pelvis"):N==TEXT("tail_mid")?TEXT("tail"):TEXT("tail_mid");
        FTransform T=Rest[N].GetRelativeTransform(Rest[Parent])*Skin->GetBoneTransformByName(Parent,EBoneSpaces::ComponentSpace);
        const float Angle=Ear?FMath::Sin(Time*4+(N==TEXT("ear_l")?0:1))*.025f: FMath::Sin(Time*(Feline?2.2f:Happy?9.f:4.f)+(N==TEXT("tail")?0:N==TEXT("tail_mid")?.45f:.9f))*(Feline?.11f:Happy?.32f:.14f);
        if(Feline&&!Ear)
        {
            const float Segment=N==TEXT("tail")?1.f:N==TEXT("tail_mid")?.7f:.45f;
            const float Balance=-TurnBalance*.28f*Segment+FMath::Sin(Time*1.2f+(1-Segment)*2)*(.02f+Moving*.02f);
            const float Carry=N==TEXT("tail")?.16f+(Happy?.12f:0):N==TEXT("tail_tip")?.16f:.035f;
            T.SetRotation(FQuat(FVector::UpVector,Balance)*FQuat(FVector::RightVector,Carry)*T.GetRotation());
        }
        else T.SetRotation(FQuat(Ear?FVector::ForwardVector:FVector::UpVector,Angle)*T.GetRotation());
        Skin->SetBoneTransformByName(N,T,EBoneSpaces::ComponentSpace);
    }
    Skin->RefreshBoneTransforms();
}
