#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MartyStance.generated.h"

class AMartyCharacter;
UENUM(BlueprintType)
enum class EMartyStance:uint8 { FourPaws, Crawl, Upright };

// Transient solo movement modes; collision changes preserve the capsule's feet.
UCLASS(ClassGroup=(Cheweenie),meta=(BlueprintSpawnableComponent))
class UMartyStance:public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) EMartyStance Active=EMartyStance::FourPaws;
    bool Supported() const;
    bool Set(EMartyStance Next,bool Recover=false);
    void Toggle(EMartyStance Mode);
    bool PrepareRun();
    void RequestJump();
    void UpdateJump(float Delta);
    void CancelJump(){JumpBuffer=0;}
    bool JumpPending() const{return JumpBuffer>0;}
    float SpeedLimit() const;
    FString Label() const;
private:
    AMartyCharacter* Pawn() const;
    void Message(const FString& Text) const;
    float JumpBuffer=0;
};
