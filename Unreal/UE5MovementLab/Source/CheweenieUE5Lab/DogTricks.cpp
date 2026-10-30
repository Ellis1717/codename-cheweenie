#include "DogLife.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

static void ShowTrickView(AMartyCharacter* P,ACameraActor* Camera)
{
    if(!Camera)return;const FVector Focus=P->GetActorLocation()+FVector(0,0,-8);
    FVector View=P->GetActorLocation()+P->GetActorRotation().RotateVector(FVector(110,-120,55));
    FHitResult Hit;FCollisionQueryParams Query(SCENE_QUERY_STAT(TrickCamera),false,P);
    if(P->GetWorld()->LineTraceSingleByChannel(Hit,Focus,View,ECC_Camera,Query))View=Hit.Location+(Focus-Hit.Location).GetSafeNormal()*15;
    Camera->GetCameraComponent()->FieldOfView=70;Camera->SetActorLocationAndRotation(View,(Focus-View).Rotation());
    if(auto* PC=Cast<APlayerController>(P->GetController()))PC->SetViewTargetWithBlend(Camera,.2f);
}

float UDogLife::Health() const{return Data()?Data()->PetHealth:100;}
float UDogLife::ActionProgress() const{return FMath::Clamp(1-ActionTime/TrickDuration,0.f,1.f);}
FString UDogLife::PaceName() const{return !Data()||Data()->PetPace==0?TEXT("Walk"):Data()->PetPace==1?TEXT("Jog"):TEXT("Run");}
float UDogLife::WalkSpeed() const{return Energy()<25?70:(SprintRequested||(Data()&&Data()->PetPace==2))?230:Data()&&Data()->PetPace==1?155:100;}
FString UDogLife::BackpackText() const
{
    if(!Data()||Data()->BagItems.IsEmpty())return TEXT("Backpack 0 / 4: empty");
    FString Result=FString::Printf(TEXT("Backpack %d / 4: "),Data()->BagItems.Num());
    for(int32 I:Data()->BagItems)Result+=(I==0?TEXT("tennis ball  "):TEXT("treasure pebble  "));return Result;
}
void UDogLife::Stow()
{
    if(!Playing()||Busy())return;auto* D=Data();
    if(D->CarryItem<0){Tell(TEXT("Pick up a toy in Marty's mouth first."));return;}
    if(D->BagItems.Num()>=4){Tell(TEXT("The backpack is full."));return;}
    D->BagItems.AddUnique(D->CarryItem);D->CarryItem=-1;SyncItems();Save();Tell(TEXT("Treasure tucked safely into the backpack."),TEXT("Toy"));
}
void UDogLife::Retrieve()
{
    if(!Playing()||Busy())return;auto* D=Data();
    if(D->CarryItem>=0){Tell(TEXT("Marty's mouth is full. Drop or bury that toy first."));return;}
    if(D->BagItems.IsEmpty()){Tell(TEXT("The backpack is empty."));return;}
    D->CarryItem=D->BagItems[0];D->BagItems.RemoveAt(0);SyncItems();Save();Tell(TEXT("Marty takes a treasure out of his backpack."),TEXT("Toy"));
}
void UDogLife::StandUp(){if(Activity==EDogActivity::Lying){Activity=EDogActivity::Idle;if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetViewTargetWithBlend(Pawn(),.2f);}}
void UDogLife::LieDown()
{
    if(!Playing()||Busy()||Pawn()->GetCharacterMovement()->IsFalling())return;
    if(Activity==EDogActivity::Lying){StandUp();Tell(TEXT("Up we get. Adventure awaits!"));return;}
    StopWatching();Pawn()->GetCharacterMovement()->StopMovementImmediately();Activity=EDogActivity::Lying;ShowTrickView(Pawn(),TVCamera);Tell(TEXT("Marty settles down. E or movement gets him back up."),TEXT("Cozy"));
}
void UDogLife::RollOver()
{
    if(!Playing()||Busy()||Pawn()->GetCharacterMovement()->IsFalling())return;
    StopWatching();Pawn()->GetCharacterMovement()->StopMovementImmediately();Activity=EDogActivity::Rolling;TrickDuration=ActionTime=2.4f;Excitement=8;ShowTrickView(Pawn(),TVCamera);Tell(TEXT("Marty Party: one extremely important barrel roll!"),TEXT("Cozy"));
}
void UDogLife::ShakePaw()
{
    if(!Playing()||Busy()||Pawn()->GetCharacterMovement()->IsFalling())return;
    StopWatching();Pawn()->GetCharacterMovement()->StopMovementImmediately();Activity=EDogActivity::Shaking;TrickDuration=ActionTime=1.8f;Excitement=8;ShowTrickView(Pawn(),TVCamera);Tell(TEXT("Mr. Marty offers his paw. Very professional."),TEXT("Cozy"));
}
void UDogLife::CyclePace()
{
    if(!Playing()||Busy())return;Data()->PetPace=(Data()->PetPace+1)%3;Save();Tell(TEXT("Pace: ")+PaceName()+TEXT(". Left stick strength still controls speed; hold A/Cross or Shift to run."));
}
void UDogLife::AdjustHealth(float Amount)
{
    if(!Data()||!FMath::IsFinite(Amount))return;Data()->PetHealth=FMath::Clamp(Data()->PetHealth+Amount,0.f,100.f);
    if(Data()->PetHealth<=0){Pawn()->ReturnHome();Data()->PetHealth=25;Data()->PetEnergy=FMath::Max(Data()->PetEnergy,25.f);Tell(TEXT("Marty returns home for a cozy recovery."));}Save();
}
