#include "MartyStance.h"
#include "MartyCharacter.h"
#include "MartyGait.h"
#include "CozyAdventure.h"
#include "DogLife.h"
#include "Components/CapsuleComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"

AMartyCharacter* UMartyStance::Pawn() const{return Cast<AMartyCharacter>(GetOwner());}
bool UMartyStance::Supported() const
{
    const auto* P=Pawn();const auto* S=P?P->Adventure():nullptr;
    return P&&S&&S->Enabled&&P->Skin&&P->Skin->IsVisible()&&P->Gait&&P->Gait->Ready;
}
void UMartyStance::Message(const FString& Text) const
{if(auto* S=Pawn()->Adventure()){S->Message=Text;S->HoldMessage(3);}}
float UMartyStance::SpeedLimit() const{return Active==EMartyStance::Crawl?45.f:Active==EMartyStance::Upright?80.f:FLT_MAX;}
FString UMartyStance::Label() const{return Active==EMartyStance::Crawl?TEXT("Crawling"):Active==EMartyStance::Upright?TEXT("Two-leg walk"):TEXT("Four paws");}
bool UMartyStance::Set(EMartyStance Next,bool Recover)
{
    auto* P=Pawn();if(!P)return false;
    if(Next==Active)return true;
    if(!Recover&&(!Supported()||!P->CanPlay()||!P->GetCharacterMovement()->IsMovingOnGround()))return false;
    auto* Capsule=P->GetCapsuleComponent();const float Old=Capsule->GetUnscaledCapsuleHalfHeight();
    const float Height=Next==EMartyStance::Crawl?22.f:Next==EMartyStance::Upright?48.f:28.f;
    const FVector Scale=Capsule->GetComponentScale();const float HeightScale=FMath::Abs(Scale.Z);
    const FVector Center=P->GetActorLocation()+FVector(0,0,(Height-Old)*HeightScale);
    if(!Recover&&Height>Old)
    {
        FCollisionQueryParams Q(SCENE_QUERY_STAT(MartyStanceHeadroom),false,P);
        const float Radius=Capsule->GetScaledCapsuleRadius();
        if(GetWorld()->OverlapBlockingTestByChannel(Center,Capsule->GetComponentQuat(),Capsule->GetCollisionObjectType(),FCollisionShape::MakeCapsule(Radius-.1f,Height*HeightScale-.1f),Q,FCollisionResponseParams(Capsule->GetCollisionResponseToChannels())))
        {Message(TEXT("Not enough headroom. Crawl out before standing or jumping."));return false;}
    }
    Active=Next;CancelJump();P->ClearControllerHurry();
    Capsule->SetCapsuleHalfHeight(Height,true);P->SetActorLocation(Center,false,nullptr,ETeleportType::TeleportPhysics);
    auto SkinLocation=P->Skin->GetRelativeLocation();SkinLocation.Z=-Height;P->Skin->SetRelativeLocation(SkinLocation);
    auto VisualLocation=P->Visual->GetRelativeLocation();VisualLocation.Z=-Height;P->Visual->SetRelativeLocation(VisualLocation);
    if(P->CameraPreset==0)P->Boom->SocketOffset.Z=Next==EMartyStance::Crawl?0:Next==EMartyStance::Upright?15:35;
    auto* Movement=P->GetCharacterMovement();Movement->bForceNextFloorCheck=true;Movement->MaxStepHeight=Next==EMartyStance::Crawl?6:Next==EMartyStance::Upright?8:12;
    Movement->MaxWalkSpeed=P->Life?P->Life->WalkSpeed():100;
    const FVector Flat=FVector(Movement->Velocity.X,Movement->Velocity.Y,0).GetClampedToMaxSize(Movement->MaxWalkSpeed);Movement->Velocity.X=Flat.X;Movement->Velocity.Y=Flat.Y;
    P->Gait->ResetContacts();if(!Recover)Message(Label()+TEXT(" | Run returns to four paws; Y / Space jumps."));return true;
}
void UMartyStance::Toggle(EMartyStance Mode){Set(Active==Mode?EMartyStance::FourPaws:Mode);}
bool UMartyStance::PrepareRun(){return Active==EMartyStance::FourPaws||Set(EMartyStance::FourPaws);}
void UMartyStance::RequestJump()
{
    auto* P=Pawn();if(!P||!P->CanPlay()||P->ControllerMenuOpen())return;
    JumpBuffer=.12f;UpdateJump(0);
}
void UMartyStance::UpdateJump(float Delta)
{
    if(JumpBuffer<=0)return;
    auto* P=Pawn();if(!P||!P->CanPlay()||P->ControllerMenuOpen()){CancelJump();return;}
    JumpBuffer-=Delta;if(JumpBuffer<0)return;
    if(!P->GetCharacterMovement()->IsMovingOnGround())return;
    if(Active==EMartyStance::Crawl&&!Set(EMartyStance::FourPaws)){CancelJump();return;}
    CancelJump();if(P->Life&&P->Life->PayJump())P->Jump();
}

void AMartyCharacter::ToggleCrawl(){if(Stances)Stances->Toggle(EMartyStance::Crawl);}
void AMartyCharacter::ToggleUpright(){if(Stances)Stances->Toggle(EMartyStance::Upright);}
bool AMartyCharacter::TryMovementShortcut(bool Upright)
{
    if(!TrickModifierDown()||!Stances||!Stances->Supported())return false;
    BeginTrickModifier();const auto* S=Adventure();if(!S||S->Screen!=ECozyScreen::Playing)return false;
    ModifierUsed=true;if(!ModifierBlocked){if(Upright)ToggleUpright();else ToggleCrawl();}return true;
}
