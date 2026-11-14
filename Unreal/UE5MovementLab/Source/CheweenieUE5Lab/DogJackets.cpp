#include "DogJackets.h"
#include "ProceduralMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkinnedAsset.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"

UDogJackets::UDogJackets()
{
    PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.bStartWithTickEnabled=false;PrimaryComponentTick.TickGroup=TG_PostUpdateWork;
}
FString UDogJackets::Name(int32 Choice)
{
    static const TCHAR* Names[]={TEXT("None"),TEXT("Rainy-Day Yellow"),TEXT("Cozy Red Fleece"),TEXT("Navy Winter"),TEXT("Pink Quilted")};
    return Names[FMath::Clamp(Choice,0,4)];
}
void UDogJackets::Shell(float Start,float End,float Width,float Height,float Z,int32 Material,float MinAngle,float MaxAngle,bool Puffy,float RearCurve,float FrontCurve,float NeckRise)
{
    auto* Mesh=NewObject<UProceduralMeshComponent>(GetOwner());Mesh->SetupAttachment(Anchor);Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);Mesh->SetGenerateOverlapEvents(false);Mesh->SetCanEverAffectNavigation(false);Mesh->bUseAsyncCooking=false;Mesh->RegisterComponent();
    const int32 Long=FMath::Max(2,FMath::CeilToInt((End-Start)/1.5f)),Around=24,Stride=Around+1,Layer=(Long+1)*Stride;
    TArray<FVector> V,N;TArray<FVector2D> UV;TArray<int32> T;
    for(int32 Side=0;Side<2;++Side)for(int32 X=0;X<=Long;++X)for(int32 A=0;A<=Around;++A)
    {
        const float U=float(X)/Long,Q=float(A)/Around,Angle=FMath::DegreesToRadians(FMath::Lerp(MinAngle,MaxAngle,Q));
        const float Edge=FMath::Pow(FMath::Abs(FMath::Sin(Angle)),1.5f);
        const float Along=FMath::Lerp(Start+RearCurve*Edge,End-FrontCurve*Edge,U),BodyU=FMath::Clamp((Along+25)/42.f,0.f,1.f);
        // All trim uses the same body taper, avoiding spikes on short seam strips.
        const float Taper=.89f+.11f*FMath::Sin(BodyU*PI),Puff=Puffy?.30f*FMath::Square(FMath::Sin(BodyU*PI*7)):.0f;
        const float W=Width*Taper-(Side?.22f:0),H=Height+Puff+NeckRise*FMath::Clamp((Along-10.5f)/7.5f,0.f,1.f)-(Side?.22f:0);
        V.Add(FVector(Along,W*FMath::Sin(Angle),Z+H*FMath::Cos(Angle)));
        N.Add(FVector(0,FMath::Sin(Angle)/W,FMath::Cos(Angle)/H).GetSafeNormal()*(Side?-1.f:1.f));UV.Add(FVector2D(U,Q));
    }
    // Unreal uses clockwise front faces; normals remain the outward surface normals.
    auto Tri=[&](int32 A,int32 B,int32 C){T.Append({A,C,B});};
    for(int32 X=0;X<Long;++X)for(int32 A=0;A<Around;++A)
    {
        const int32 I=X*Stride+A;Tri(I,I+Stride,I+1);Tri(I+1,I+Stride,I+Stride+1);
        Tri(I+Layer,I+1+Layer,I+Stride+Layer);Tri(I+1+Layer,I+Stride+1+Layer,I+Stride+Layer);
    }
    auto Edge=[&](int32 A,int32 B){Tri(A,B,A+Layer);Tri(B,B+Layer,A+Layer);};
    for(int32 X=0;X<Long;++X){Edge(X*Stride,(X+1)*Stride);Edge((X+1)*Stride+Around,X*Stride+Around);}
    for(int32 A=0;A<Around;++A){Edge(A+1,A);Edge(Long*Stride+A,Long*Stride+A+1);}
    Mesh->CreateMeshSection_LinearColor(0,V,T,N,UV,TArray<FLinearColor>(),TArray<FProcMeshTangent>(),false);
    Mesh->SetMaterial(0,Palette[Material]);Pieces.Add(Mesh);
}
void UDogJackets::Build()
{
    for(const auto& P:Pieces)P->DestroyComponent();Pieces.Empty();Palette.Empty();
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Adventure/World/NeighbourhoodSurface.NeighbourhoodSurface"));
    const FLinearColor Bodies[]={FLinearColor(1,.66f,.015f),FLinearColor(.48f,.015f,.035f),FLinearColor(.025f,.065f,.14f),FLinearColor(.72f,.10f,.29f)};
    const auto Body=Bodies[Style-1];
    for(const auto Color:{Body,FLinearColor(.018f,.025f,.045f),FLinearColor(.78f,.82f,.78f),FLinearColor(.36f,.025f,.10f),FLinearColor(.75f,.59f,.40f)})
    {auto* M=UMaterialInstanceDynamic::Create(Base,this);M->SetVectorParameterValue(TEXT("Color"),Color);Palette.Add(M);}
    // The open underside and shoulder cut leave all four legs, face and tail free.
    Shell(-25,17,10.7f,9.8f,2,0,-108,108,Style>=3,3,2.5f);
    Shell(-6,2,10.6f,8.8f,2,1,90,270); // Belly fastening, between the legs.
    const int32 Collar=Style==1?0:1;
    Shell(10.5f,18,11.05f,10.f,2.5f,Collar,-100,100,Style>=3,0,0,2.8f);
    if(Style==1)
    {
        // Reflective-style piping and two original back stripes; no product logo.
        Shell(-25,-24.2f,10.9f,10.0f,2,2,-108,108,false,3,-3);
        for(float Side:{-1.f,1.f})Shell(-24,16,10.95f,10.05f,2,2,Side<0?-108:104,Side<0?-104:108,false,3,2.5f);
        Shell(-18,9,10.95f,10.05f,2,2,-4,4);
    }
    if(Style==2)
    {
        // Raised narrow ribs suggest the fleece collar rather than a hard neck ring.
        for(int32 I=0;I<7;++I)Shell(11+I*.9f,11.45f+I*.9f,11.3f,10.3f,2.5f,1,-100,100,false,0,0,2.8f);
        Shell(-25,-24.25f,10.95f,10.05f,2,4,-108,108,false,3,-3);
    }
    if(Style>=3)
    {
        for(float X:{-20.f,-14.f,-8.f,-2.f,4.f})Shell(X,X+.45f,10.9f,10.35f,2,Style==4?3:1);
        Shell(17.4f,18.2f,11.3f,10.4f,2.5f,Style==4?0:2,-100,100,false,0,0,2.8f);
    }
}
void UDogJackets::SetJacket(USceneComponent* PetMesh,int32 Choice)
{
    Choice=FMath::Clamp(Choice,0,4);
    if(!IsValid(PetMesh)||Choice==0){Style=0;if(Anchor)Anchor->SetVisibility(false,true);SetComponentTickEnabled(false);return;}
    if(Style==Choice&&Target==PetMesh)return;
    if(!Anchor){Anchor=NewObject<USceneComponent>(GetOwner());Anchor->SetupAttachment(GetOwner()->GetRootComponent());Anchor->RegisterComponent();}
    const bool Rebuild=Style!=Choice||Pieces.IsEmpty();Style=Choice;Target=PetMesh;if(Rebuild)Build();
    Anchor->SetVisibility(true,true);SetComponentTickEnabled(true);TickComponent(0,LEVELTICK_All,nullptr);
}
void UDogJackets::TickComponent(float Delta,ELevelTick Type,FActorComponentTickFunction* Function)
{
    Super::TickComponent(Delta,Type,Function);if(!Style||!IsValid(Target))return;
    if(auto* Skin=Cast<UPoseableMeshComponent>(Target.Get()))
    {
        if(!Attachment.Resolve(Skin->GetSkinnedAsset()))return;
        FTransform Follow=Attachment.RelativeSaddle*Skin->GetBoneTransformByName(TEXT("spine"),EBoneSpaces::WorldSpace);Follow.SetScale3D(FVector::OneVector);Anchor->SetWorldTransform(Follow);
    }
    else {const auto Transform=Target->GetComponentTransform();Anchor->SetWorldTransform(FTransform(Transform.GetRotation(),Transform.TransformPosition(FVector(-15,0,82)),FVector::OneVector));}
}
int32 UDogJackets::VisiblePieces() const{int32 N=0;for(const auto& P:Pieces)if(P->IsVisible())++N;return N;}
bool UDogJackets::CollisionFree() const{for(const auto& P:Pieces)if(P->GetCollisionEnabled()!=ECollisionEnabled::NoCollision||P->GetGenerateOverlapEvents()||P->CanEverAffectNavigation())return false;return true;}
bool UDogJackets::MaterialsReady() const{if(Pieces.IsEmpty())return false;for(const auto& M:Palette)if(!M||!M->Parent)return false;return true;}
bool UDogJackets::SurfaceFacesOutward() const
{
    if(Pieces.IsEmpty())return false;
    for(const auto& P:Pieces)
    {
        const auto* Section=P->GetProcMeshSection(0);if(!Section||Section->ProcIndexBuffer.Num()<3)return false;
        const auto& A=Section->ProcVertexBuffer[Section->ProcIndexBuffer[0]];const auto& B=Section->ProcVertexBuffer[Section->ProcIndexBuffer[1]];const auto& C=Section->ProcVertexBuffer[Section->ProcIndexBuffer[2]];
        if(FVector::DotProduct(FVector::CrossProduct(B.Position-A.Position,C.Position-A.Position),A.Normal)>=0)return false;
    }
    return true;
}
FTransform UDogJackets::JacketTransform() const{return Anchor?Anchor->GetComponentTransform():FTransform::Identity;}
