#include "MartyGait.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
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
    Trot=FMath::FInterpTo(Trot,SmoothedSpeed>65?1.f:0.f,Delta,4);
    const float Stride=FMath::Lerp(WalkStride,TrotStride,Trot);
    Cycle=FMath::Fmod(Cycle+Distance/FMath::Max(Stride,1.f),1.f);
    const float Moving=FMath::Clamp(SmoothedSpeed/25.f,0.f,1.f);
    const float Bob=FMath::Sin(Cycle*UE_TWO_PI*2)*.45f*Moving+FMath::Sin(Time*2.1f)*.16f*(1-Moving);
    FTransform Spine=Rest[TEXT("spine")];Spine.AddToTranslation(FVector(0,0,Bob));Skin->SetBoneTransformByName(TEXT("spine"),Spine,EBoneSpaces::ComponentSpace);
    FTransform Head=Rest[TEXT("head")];Head.AddToTranslation(FVector(0,0,Bob));
    Head.SetRotation(FQuat(FVector(0,1,0),FMath::Sin(Cycle*UE_TWO_PI*2)*.025f*Moving)*Head.GetRotation());
    Skin->SetBoneTransformByName(TEXT("head"),Head,EBoneSpaces::ComponentSpace);
    for(const FName Name:{FName(TEXT("ear_l")),FName(TEXT("ear_r")),FName(TEXT("ear_tip_l")),FName(TEXT("ear_tip_r")),FName(TEXT("tail"))})
    {
        FTransform Part=Rest[Name];Part.AddToTranslation(FVector(0,0,Bob));
        const bool Tail=Name==TEXT("tail"),Tip=Name==TEXT("ear_tip_l")||Name==TEXT("ear_tip_r");
        const float Side=Name==TEXT("ear_l")||Name==TEXT("ear_tip_l")?-1.f:1.f;
        const float Flap=Tail?FMath::Sin(Time*3.f)*.10f:FMath::Sin(Cycle*UE_TWO_PI*2+(Tip?.7f:0))*Side*(Tip?.27f:.15f)*Moving;
        Part.SetRotation(FQuat(Tail?FVector::UpVector:FVector(1,0,0),Flap)*Part.GetRotation());
        if(Tip)
        {
            const FName Parent=Side<0?TEXT("ear_l"):TEXT("ear_r");
            const FTransform Base=Skin->GetBoneTransformByName(Parent,EBoneSpaces::ComponentSpace);
            Part.SetLocation(Base.TransformPosition(Rest[Parent].InverseTransformPosition(Rest[Name].GetLocation())));
        }
        Skin->SetBoneTransformByName(Name,Part,EBoneSpaces::ComponentSpace);
    }
    const float WalkPhases[]={0,.5f,.75f,.25f};const float TrotPhases[]={0,.5f,.5f,0};
    const FTransform World=Skin->GetComponentTransform();PlantedPaws=0;MaxJointBend=0;
    for(int32 I=0;I<Legs.Num();++I)
    {
        auto& Leg=Legs[I];const float Phase=FMath::Fmod(Cycle+FMath::Lerp(WalkPhases[I],TrotPhases[I],Trot),1.f);
        const float Stance=FMath::Lerp(.70f,.52f,Trot);const bool Plant=Phase<Stance || Moving<.05f;
        FVector RestWorld=World.TransformPosition(Leg.Toe);
        FVector Target=Ground(RestWorld+Pawn->GetVelocity().GetClampedToMaxSize(100)*.09f);
        if(Plant)
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
        const FVector Toe=World.InverseTransformPosition(Leg.Contact);
        FVector Hip=Leg.Hip+FVector(0,0,Bob);
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
    Skin->RefreshBoneTransforms();
}
