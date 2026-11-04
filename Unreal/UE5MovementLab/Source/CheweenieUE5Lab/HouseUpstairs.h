#pragma once
#include "CoreMinimal.h"
class UWorld;

namespace CheweenieHouse
{
    constexpr float UpperFloor = 380;
    inline FVector StairBottom(){return FVector(-390,-420,28);}
    inline FVector StairTop(){return FVector(435,-280,408);}
    inline bool Upstairs(FVector P){return P.Z>UpperFloor-25&&FMath::Abs(P.X)<510&&FMath::Abs(P.Y)<510;}
    inline bool SameFloor(FVector A,FVector B){return FMath::Abs(A.Z-B.Z)<120;}
    inline bool Near(FVector A,FVector B,float Radius){return SameFloor(A,B)&&FVector::Dist2D(A,B)<Radius;}
    void Build(UWorld* World);
    FString Location(FVector P);
}
