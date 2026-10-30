#include "DogLife.h"
#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

bool UDogLife::UsesBodyControls() const{return Data()&&Data()->BodyControls;}
void UDogLife::SetBodyStick(FVector2D Value){BodyStick=Value.GetClampedToMaxSize(1);}
void UDogLife::ClearBodyStick(){BodyStick=FVector2D::ZeroVector;}
void UDogLife::ToggleBodyControls()
{
    if(!Playing()||Busy())return;Data()->BodyControls=!Data()->BodyControls;ClearBodyStick();BodyRoll=BodyPitch=BodyYawSpeed=0;BodyVelocity=FVector2D::ZeroVector;
    Pawn()->GetCharacterMovement()->bOrientRotationToMovement=!UsesBodyControls();if(auto* PC=Cast<APlayerController>(Pawn()->GetController()))PC->SetControlRotation(FRotator(-12,Pawn()->GetActorRotation().Yaw,0));Save();
    Tell(UsesBodyControls()?TEXT("Body controls: left stick steers; right stick swings and turns. C / B-Circle switches back."):TEXT("Standard controls restored: right stick moves the camera."));
}
void UDogLife::TickBodyControls(float Delta)
{
    auto* P=Pawn();P->GetCharacterMovement()->bOrientRotationToMovement=!UsesBodyControls();
    const FVector2D Input=UsesBodyControls()&&Activity==EDogActivity::Idle?BodyStick:FVector2D::ZeroVector;
    const FVector2D Target(Input.X*(Energy()<25?8:12),Input.Y*(Energy()<25?6:8));
    // Bounded mass/spring/damper integration; substeps tolerate frame hitches.
    float Remaining=FMath::Min(Delta,.1f);FVector2D Angle(BodyRoll,BodyPitch);
    while(Remaining>0){const float H=FMath::Min(Remaining,.008f);BodyVelocity+=((Target-Angle)*50-BodyVelocity*14)*H;Angle+=BodyVelocity*H;Remaining-=H;}
    BodyRoll=FMath::Clamp(float(Angle.X),-12.f,12.f);BodyPitch=FMath::Clamp(float(Angle.Y),-8.f,8.f);
    BodyYawSpeed=FMath::FInterpTo(BodyYawSpeed,Input.X*(Energy()<25?60:90),Delta,4);
    if(UsesBodyControls()&&Activity==EDogActivity::Idle)
    {
        FRotator R=P->GetActorRotation();R.Yaw+=BodyYawSpeed*Delta;P->SetActorRotation(R);
        if(auto* PC=Cast<APlayerController>(P->GetController())){FRotator View=PC->GetControlRotation();View.Yaw=FMath::FixedTurn(View.Yaw,R.Yaw,120*Delta);PC->SetControlRotation(View);}
    }
}
