#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundBase.h"

// Separate assets preserve the synthesized pack as a fallback.
inline void LoadNaturalBarks(TArray<TObjectPtr<USoundBase>>& Takes)
{
    for(int32 I=1;I<=4;++I)
        if(auto* Sound=LoadObject<USoundBase>(nullptr,*FString::Printf(TEXT("/Game/Adventure/Audio/NaturalPets/Bark%02d.Bark%02d"),I,I))) Takes.Add(Sound);
}

inline USoundBase* ChooseNaturalBark(const TArray<TObjectPtr<USoundBase>>& Takes,int32& Last)
{
    if(Takes.IsEmpty())return nullptr;
    int32 Next=FMath::RandHelper(Takes.Num()-(Takes.IsValidIndex(Last)&&Takes.Num()>1?1:0));
    if(Takes.Num()>1&&Takes.IsValidIndex(Last)&&Next>=Last)++Next;
    Last=Next;return Takes[Next];
}

inline USoundBase* NaturalCatVoice(const FString& Name)
{
    return LoadObject<USoundBase>(nullptr,*(TEXT("/Game/Adventure/Audio/NaturalPets/")+Name+TEXT(".")+Name));
}
