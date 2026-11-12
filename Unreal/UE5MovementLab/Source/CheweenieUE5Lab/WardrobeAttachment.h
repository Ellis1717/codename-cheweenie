#pragma once
#include "CoreMinimal.h"
#include "Engine/SkinnedAsset.h"

// Reference-pose work is immutable for a runtime asset. Live bone poses are still
// sampled by the wearer every frame. Key by asset, since fur swaps reuse a component.
struct FWardrobeAttachment
{
    bool Resolve(const USkinnedAsset* Asset)
    {
        if (!Asset) return false;
        if (CachedAsset.Get() == Asset) return HasSpine;
        CachedAsset = Asset;
        ++Builds;
        const auto& Skeleton = Asset->GetRefSkeleton();
        const int32 Index = Skeleton.FindBoneIndex(TEXT("spine"));
        HasSpine = Index != INDEX_NONE;
        if (!HasSpine) return false;
        FTransform Bind = Skeleton.GetRefBonePose()[Index];
        for (int32 Parent = Skeleton.GetParentIndex(Index); Parent != INDEX_NONE; Parent = Skeleton.GetParentIndex(Parent))
            Bind = Bind * Skeleton.GetRefBonePose()[Parent];
        const FTransform Saddle(FQuat::Identity, Bind.GetLocation() + FVector(5, 0, 4));
        RelativeSaddle = Saddle.GetRelativeTransform(Bind);
        return true;
    }

    FTransform RelativeSaddle = FTransform::Identity;
    int32 Builds = 0;
private:
    TWeakObjectPtr<const USkinnedAsset> CachedAsset;
    bool HasSpine = false;
};
