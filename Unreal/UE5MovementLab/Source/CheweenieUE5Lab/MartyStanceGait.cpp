#include "MartyGait.h"
#include "MartyCharacter.h"
#include "MartyStance.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMartyGait::ApplyMovementStance(float Delta)
{
    const auto* P=Cast<AMartyCharacter>(GetOwner());
    const bool Up=P&&P->Stances&&P->Stances->Active==EMartyStance::Upright;
    UprightBlend=FMath::FInterpTo(UprightBlend,Up?1.f:0.f,Delta,10);
    const float Bounds=UprightBlend>.001f?3.f:1.f;if(!FMath::IsNearlyEqual(Skin->BoundsScale,Bounds))Skin->SetBoundsScale(Bounds);
    if(UprightBlend<.001f||Legs.Num()!=4)return;
    const int32 Count=Skin->GetNumBones();TArray<FTransform> Normal;Normal.Reserve(Count);
    for(int32 I=0;I<Count;++I)Normal.Add(Skin->GetBoneTransformByName(Skin->GetBoneName(I),EBoneSpaces::ComponentSpace));
    const FVector Pivot=(Legs[2].Hip+Legs[3].Hip)*.5f;
    const FQuat Tilt(FVector::YAxisVector,FMath::DegreesToRadians(-70.f));
    const FVector Shift(-Pivot.X,0,0);
    auto Point=[&](FVector V){return Pivot+Tilt.RotateVector(V-Pivot)+Shift;};
    for(int32 I=0;I<Count;++I)
    {
        auto B=Normal[I];B.SetLocation(Point(B.GetLocation()));B.SetRotation(Tilt*B.GetRotation());
        Skin->SetBoneTransformByName(Skin->GetBoneName(I),B,EBoneSpaces::ComponentSpace);
    }
    // The chest rises while the face continues looking ahead. Ears follow the
    // counter-rotated head; the tail stays clear of the floor behind the pelvis.
    const int32 HeadIndex=Skin->GetBoneIndex(TEXT("head"));
    FTransform Head=Skin->GetBoneTransformByName(TEXT("head"),EBoneSpaces::ComponentSpace);
    Head.SetRotation(FQuat(FVector::YAxisVector,FMath::DegreesToRadians(-10.f))*Normal[HeadIndex].GetRotation());
    Skin->SetBoneTransformByName(TEXT("head"),Head,EBoneSpaces::ComponentSpace);
    for(const FName Ear:{FName(TEXT("ear_l")),FName(TEXT("ear_r"))})
    {
        const FName Tip=Ear==TEXT("ear_l")?TEXT("ear_tip_l"):TEXT("ear_tip_r");
        const FTransform EarPose=Normal[Skin->GetBoneIndex(Ear)].GetRelativeTransform(Normal[HeadIndex])*Head;
        Skin->SetBoneTransformByName(Ear,EarPose,EBoneSpaces::ComponentSpace);
        const auto TipPose=Normal[Skin->GetBoneIndex(Tip)].GetRelativeTransform(Normal[Skin->GetBoneIndex(Ear)])*EarPose;
        Skin->SetBoneTransformByName(Tip,TipPose,EBoneSpaces::ComponentSpace);
    }
    FTransform Tail=Normal[Skin->GetBoneIndex(TEXT("tail"))];FVector TailPoint=Point(Tail.GetLocation());TailPoint.Z=FMath::Max(TailPoint.Z,12.f);
    Tail.SetLocation(TailPoint);Tail.SetRotation(FQuat(FVector::YAxisVector,FMath::DegreesToRadians(-20.f))*Tail.GetRotation());Skin->SetBoneTransformByName(TEXT("tail"),Tail,EBoneSpaces::ComponentSpace);
    int32 Supports=0;const FTransform World=Skin->GetComponentTransform();
    for(int32 I=0;I<4;++I)
    {
        auto& Leg=Legs[I];const FVector Hip=Point(Leg.Hip);
        FVector Toe;
        if(I<2)
        {
            const FVector Chest=(Point(Legs[0].Hip)+Point(Legs[1].Hip))*.5f;
            Toe=FVector(Chest.X+9,Leg.Toe.Y,Chest.Z-10)+FVector(FMath::Sin(Cycle*UE_TWO_PI+I*UE_PI)*2,0,0);
            Leg.Planted=false;
        }
        else
        {
            Toe=World.InverseTransformPosition(Leg.Contact)+Shift;
            if(!P->GetCharacterMovement()->IsFalling())Toe=World.InverseTransformPosition(Ground(World.TransformPosition(Toe)));
            if(Leg.Planted)++Supports;
        }
        const float Upper=(Leg.Knee-Leg.Hip).Size(),Lower=(Leg.Ankle-Leg.Knee).Size();
        FVector Ankle=Toe+(I<2?Tilt.RotateVector(Leg.Ankle-Leg.Toe):(Leg.Ankle-Leg.Toe));
        Ankle=Hip+(Ankle-Hip).GetClampedToMaxSize(Upper+Lower-.05f);
        const FVector Knee=SolveKnee(Hip,Ankle,Upper,Lower,I<2?1.f:-1.f);
        Aim(Leg.Upper,Hip,Leg.Knee,Knee);Aim(Leg.Lower,Knee,Leg.Ankle,Ankle);Aim(Leg.Hock,Ankle,Leg.Toe,Toe);
        FTransform Foot=Rest[Leg.Paw];Foot.SetLocation(Toe);Skin->SetBoneTransformByName(Leg.Paw,Foot,EBoneSpaces::ComponentSpace);
    }
    TArray<FTransform> Upright;Upright.Reserve(Count);
    for(int32 I=0;I<Count;++I)Upright.Add(Skin->GetBoneTransformByName(Skin->GetBoneName(I),EBoneSpaces::ComponentSpace));
    for(int32 I=0;I<Count;++I)
    {
        FTransform Blended;Blended.Blend(Normal[I],Upright[I],UprightBlend);
        Skin->SetBoneTransformByName(Skin->GetBoneName(I),Blended,EBoneSpaces::ComponentSpace);
    }
    if(UprightBlend>.5f)PlantedPaws=Supports;
}
