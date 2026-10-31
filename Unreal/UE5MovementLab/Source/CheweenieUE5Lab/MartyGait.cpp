#include "MartyGait.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"

UMartyGait::UMartyGait(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickGroup=TG_PostPhysics;}
void UMartyGait::BeginPlay()
{
    Super::BeginPlay();
    Skin=GetOwner()->FindComponentByClass<UPoseableMeshComponent>();
    if(!Skin || !Skin->GetSkinnedAsset()) return;
    const TCHAR* Required[]={TEXT("root"),TEXT("spine"),TEXT("head"),TEXT("tail"),TEXT("ear_l"),TEXT("ear_r"),TEXT("ear_tip_l"),TEXT("ear_tip_r")};
    for(const auto* Name:Required)
    {
        if(Skin->GetBoneIndex(Name)==INDEX_NONE) return;
        Rest.Add(Name,Skin->GetBoneTransformByName(Name,EBoneSpaces::ComponentSpace));
    }
    for(int32 Index=0;Index<4;++Index)
    {
        const FString Prefix=FString(Index<2?TEXT("front"):TEXT("hind"))+(Index%2==0?TEXT("_l"):TEXT("_r"));
        FMartyLeg Leg; FName* Names[]={&Leg.Upper,&Leg.Lower,&Leg.Hock,&Leg.Paw};
        const TCHAR* Suffix[]={TEXT("_upper"),TEXT("_lower"),TEXT("_hock"),TEXT("_paw")};
        for(int32 J=0;J<4;++J)
        {
            *Names[J]=FName(Prefix+Suffix[J]);
            if(Skin->GetBoneIndex(*Names[J])==INDEX_NONE) return;
            Rest.Add(*Names[J],Skin->GetBoneTransformByName(*Names[J],EBoneSpaces::ComponentSpace));
        }
        Leg.Hip=Rest[Leg.Upper].GetLocation();Leg.Knee=Rest[Leg.Lower].GetLocation();
        Leg.Ankle=Rest[Leg.Hock].GetLocation();Leg.Toe=Rest[Leg.Paw].GetLocation();Legs.Add(Leg);
    }
    Ready=true;ResetContacts();
}
void UMartyGait::ResetContacts()
{
    LastPosition=GetOwner()->GetActorLocation();
    for(auto& Leg:Legs){Leg.Planted=false;Leg.Contact=Ground(Skin->GetComponentTransform().TransformPosition(Leg.Toe));Leg.LiftOff=Leg.Contact;}
}
FVector UMartyGait::MouthWorld() const
{
    if(!Ready)return GetOwner()->GetActorTransform().TransformPosition(FVector(40,0,5));
    const FVector Socket=Rest[TEXT("head")].InverseTransformPosition(FVector(40,0,33));
    return Skin->GetBoneTransformByName(TEXT("head"),EBoneSpaces::WorldSpace).TransformPosition(Socket);
}
FVector UMartyGait::Ground(FVector Point) const
{
    FHitResult Hit; FCollisionQueryParams Params(SCENE_QUERY_STAT(MartyPaw),false,GetOwner());
    if(GetWorld()->LineTraceSingleByChannel(Hit,Point+FVector(0,0,36),Point-FVector(0,0,70),ECC_Visibility,Params))
        Point.Z=Hit.ImpactPoint.Z+2.4f;
    return Point;
}
FVector UMartyGait::SolveKnee(FVector Hip,FVector Foot,float Upper,float Lower,float Direction)
{
    FVector Ray=Foot-Hip;float Distance=FMath::Clamp(Ray.Size(),.1f,Upper+Lower-.02f);Ray.Normalize();
    const float Along=(Upper*Upper-Lower*Lower+Distance*Distance)/(2*Distance);
    FVector Pole=FVector(Direction,0,0);Pole=(Pole-Ray*FVector::DotProduct(Pole,Ray)).GetSafeNormal();
    return Hip+Ray*Along+Pole*FMath::Sqrt(FMath::Max(0.f,Upper*Upper-Along*Along));
}
void UMartyGait::Aim(FName Bone,FVector Origin,FVector RestEnd,FVector End)
{
    FTransform Transform=Rest[Bone];
    Transform.SetRotation(FQuat::FindBetweenNormals((RestEnd-Rest[Bone].GetLocation()).GetSafeNormal(),(End-Origin).GetSafeNormal())*Transform.GetRotation());
    Transform.SetLocation(Origin);Skin->SetBoneTransformByName(Bone,Transform,EBoneSpaces::ComponentSpace);
}
void UMartyGait::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);if(!Ready) return;
    auto* Pawn=Cast<AMartyCharacter>(GetOwner());if(!Pawn)return;
    auto* Mode=GetWorld()->GetAuthGameMode<AMartyPlayMode>();
    const bool Playing=!Mode || !Mode->Adventure || Mode->Adventure->AllowsMovement();
    FVector Position=Pawn->GetActorLocation();float Distance=FVector::Dist2D(Position,LastPosition);
    if(Distance>80){ResetContacts();Distance=0;}LastPosition=Position;
    if(!Playing)return; // Pause/menu cannot keep walking or advance animation time.
    Time+=Delta;Travel+=Distance;
    SmoothedSpeed=FMath::FInterpTo(SmoothedSpeed,Pawn->GetVelocity().Size2D(),Delta,12);
    Trot=FMath::FInterpTo(Trot,SmoothedSpeed>120?1.f:0.f,Delta,4);
    const float Run=FMath::Clamp((SmoothedSpeed-165)/60,0.f,1.f);
    const float Stride=FMath::Lerp(FMath::Lerp(WalkStride,TrotStride,Trot),56.f,Run);
    const bool Air=Pawn->GetCharacterMovement()->IsFalling();
    const bool Digging=Pawn->Life&&Pawn->Life->Activity==EDogActivity::Digging;
    const bool Eating=Pawn->Life&&Pawn->Life->Activity==EDogActivity::Eating;
    const bool Watching=Pawn->Life&&Pawn->Life->Activity==EDogActivity::WatchingTV;
    const bool Lying=Pawn->Life&&Pawn->Life->Activity==EDogActivity::Lying;
    const bool Rolling=Pawn->Life&&Pawn->Life->Activity==EDogActivity::Rolling;
    const bool Shaking=Pawn->Life&&Pawn->Life->Activity==EDogActivity::Shaking;
    const bool Tired=Pawn->Life&&Pawn->Life->Energy()<25;
    const bool Excited=Pawn->Life&&Pawn->Life->Mood()==TEXT("Excited");
    Cycle=FMath::Fmod(Cycle+(Air?0:Distance)/FMath::Max(Stride,1.f),1.f);
    const float Moving=FMath::Clamp(SmoothedSpeed/25.f,0.f,1.f);
    RestLowering=FMath::FInterpTo(RestLowering,Lying?18.f:Rolling?9.f:Shaking?5.f:Watching?5.f:Eating?6.f:Tired?1.f:0.f,Delta,8);
    const float Bob=FMath::Sin(Cycle*UE_TWO_PI*2)*(.45f+Run*.5f)*Moving+FMath::Sin(Time*2.1f)*.16f*(1-Moving)-RestLowering;
    Skin->SetBoneTransformByName(TEXT("root"),Rest[TEXT("root")],EBoneSpaces::ComponentSpace);
    const FQuat Lean=Pawn->Life&&!Rolling&&!Shaking?FQuat(FVector(1,0,0),FMath::DegreesToRadians(Pawn->Life->BodyRoll))*FQuat(FVector(0,1,0),FMath::DegreesToRadians(Pawn->Life->BodyPitch)):FQuat::Identity;
    auto LeanPoint=[&](FVector Point){const FVector Pivot(0,0,24);return Pivot+Lean.RotateVector(Point-Pivot);};
    FTransform Spine=Rest[TEXT("spine")];Spine.SetLocation(LeanPoint(Spine.GetLocation()+FVector(0,0,Bob)));Spine.SetRotation(Lean*Spine.GetRotation());Skin->SetBoneTransformByName(TEXT("spine"),Spine,EBoneSpaces::ComponentSpace);
    FTransform Head=Rest[TEXT("head")];Head.SetLocation(LeanPoint(Head.GetLocation()+FVector(0,0,Bob)));
    Head.SetRotation(Lean*FQuat(FVector(0,1,0),FMath::Sin(Cycle*UE_TWO_PI*2)*.025f*Moving+(Eating?.65f:Digging?.35f:Tired?.09f:0))*Head.GetRotation());
    Skin->SetBoneTransformByName(TEXT("head"),Head,EBoneSpaces::ComponentSpace);
    for(const FName Name:{FName(TEXT("ear_l")),FName(TEXT("ear_r")),FName(TEXT("ear_tip_l")),FName(TEXT("ear_tip_r")),FName(TEXT("tail"))})
    {
        FTransform Part=Rest[Name];Part.SetLocation(LeanPoint(Part.GetLocation()+FVector(0,0,Bob)));
        const bool Tail=Name==TEXT("tail"),Tip=Name==TEXT("ear_tip_l")||Name==TEXT("ear_tip_r");
        const float Side=Name==TEXT("ear_l")||Name==TEXT("ear_tip_l")?-1.f:1.f;
        const float Flap=Tail?FMath::Sin(Time*(Excited?5.f:3.f))*(Excited?.18f:.10f):FMath::Sin(Cycle*UE_TWO_PI*2+(Tip?.7f:0))*Side*(Tip?.27f:.15f)*Moving;
        Part.SetRotation(Lean*FQuat(Tail?FVector::UpVector:FVector(1,0,0),Flap)*Part.GetRotation());
        if(Tip)
        {
            const FName Parent=Side<0?TEXT("ear_l"):TEXT("ear_r");
            const FTransform Base=Skin->GetBoneTransformByName(Parent,EBoneSpaces::ComponentSpace);
            Part.SetLocation(Base.TransformPosition(Rest[Parent].InverseTransformPosition(Rest[Name].GetLocation())));
        }
        Skin->SetBoneTransformByName(Name,Part,EBoneSpaces::ComponentSpace);
    }
    const float WalkPhases[]={0,.5f,.75f,.25f};const float TrotPhases[]={0,.5f,.5f,0};const float RunPhases[]={0,.14f,.58f,.72f};
    const FTransform World=Skin->GetComponentTransform();PlantedPaws=0;MaxJointBend=0;
    for(int32 I=0;I<Legs.Num();++I)
    {
        auto& Leg=Legs[I];const float Phase=FMath::Fmod(Cycle+FMath::Lerp(FMath::Lerp(WalkPhases[I],TrotPhases[I],Trot),RunPhases[I],Run),1.f);
        const float Stance=FMath::Lerp(FMath::Lerp(.70f,.52f,Trot),.40f,Run);const bool Plant=Phase<Stance || Moving<.05f;
        FVector RestWorld=World.TransformPosition(Leg.Toe);
        FVector Target=Ground(RestWorld+Pawn->GetVelocity().GetClampedToMaxSize(100)*.09f);
        if(Air||(Digging&&I<2))
        {
            Leg.Planted=false;Leg.Contact=RestWorld+FVector(Digging?FMath::Sin(Time*18+I*UE_PI)*4:0,0,Air?7:4+FMath::Sin(Time*18+I*UE_PI)*3);
        }
        else if(Plant)
        {
            if(!Leg.Planted)Leg.Contact=Ground(Leg.Contact);
            // Reset a contact after a sharp turn/teleport rather than stretching the skin.
            if(FVector::Dist2D(Leg.Contact,RestWorld)>Stride*.65f)Leg.Contact=Ground(RestWorld);
            Leg.LiftOff=Leg.Contact;Leg.Planted=true;++PlantedPaws;
        }
        else
        {
            if(Leg.Planted){Leg.LiftOff=Leg.Contact;Leg.Planted=false;}
            const float T=(Phase-Stance)/(1-Stance);const float Smooth=T*T*(3-2*T);
            Leg.Contact=FMath::Lerp(Leg.LiftOff,Target,Smooth)+FVector(0,0,FMath::Sin(T*UE_PI)*PawLift*Moving);
        }
        FVector Toe=World.InverseTransformPosition(Leg.Contact);
        if(!Rolling&&!Shaking)Toe.X+=(I<2?12:3)*FMath::Clamp((RestLowering-9)/9,0.f,1.f);
        if(Rolling){Toe=Leg.Toe+FVector(I<2?-3:3,0,12);Leg.Planted=false;}
        if(Shaking&&I==0){const float T=Pawn->Life->ActionProgress();const float Lift=FMath::Clamp(T/.18f,0.f,1.f)*FMath::Clamp((1-T)/.18f,0.f,1.f);Toe=Leg.Toe+FVector(9+FMath::Sin(Time*8)*1.5f,0,15)*Lift;Leg.Planted=false;}
        FVector Hip=LeanPoint(Leg.Hip+FVector(0,0,Bob));
        FVector Ankle=Toe+(Leg.Ankle-Leg.Toe);
        const float Upper=(Leg.Knee-Leg.Hip).Size(),Lower=(Leg.Ankle-Leg.Knee).Size();
        // Clamp extension before solving; unreachable contacts cannot invert the knee.
        Ankle=Hip+(Ankle-Hip).GetClampedToMaxSize(Upper+Lower-.05f);
        const FVector Knee=SolveKnee(Hip,Ankle,Upper,Lower,I<2?-1.f:1.f);
        Aim(Leg.Upper,Hip,Leg.Knee,Knee);Aim(Leg.Lower,Knee,Leg.Ankle,Ankle);Aim(Leg.Hock,Ankle,Leg.Toe,Toe);
        FTransform Foot=Rest[Leg.Paw];Foot.SetLocation(Toe);Skin->SetBoneTransformByName(Leg.Paw,Foot,EBoneSpaces::ComponentSpace);
        const float Bend=FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct((Knee-Hip).GetSafeNormal(),(Ankle-Knee).GetSafeNormal()),-1.f,1.f)));
        MaxJointBend=FMath::Max(MaxJointBend,Bend);
    }
    if(Rolling)
    {
        const float T=Pawn->Life->ActionProgress();const float Smooth=T*T*(3-2*T);
        const FQuat Roll(FVector(1,0,0),-Smooth*UE_TWO_PI);const FVector Pivot(0,0,15);
        TArray<FTransform> Pose;for(int32 I=0;I<Skin->GetNumBones();++I)Pose.Add(Skin->GetBoneTransformByName(Skin->GetBoneName(I),EBoneSpaces::ComponentSpace));
        for(int32 I=0;I<Pose.Num();++I){FTransform B=Pose[I];B.SetLocation(Pivot+Roll.RotateVector(B.GetLocation()-Pivot));B.SetRotation(Roll*B.GetRotation());Skin->SetBoneTransformByName(Skin->GetBoneName(I),B,EBoneSpaces::ComponentSpace);}
    }
    if(Rolling)PlantedPaws=0;else if(Shaking)PlantedPaws=FMath::Max(0,PlantedPaws-1);
    Skin->RefreshBoneTransforms();
}
