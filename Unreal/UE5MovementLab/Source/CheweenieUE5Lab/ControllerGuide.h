#pragma once
#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ControllerGuide.generated.h"

class IInputProcessor;
class AHUD;
enum class ECozyInputFamily : uint8 { Keyboard, Xbox, PlayStation, Generic };

// Presentation only: never consumes events or changes gameplay mappings.
UCLASS()
class UControllerGuide : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    void Observe(FKey Key, FInputDeviceId Device, float Amount=1.f);
    static ECozyInputFamily Classify(const FString& Name);
    ECozyInputFamily Family() const;
    void CycleOverride();
    FString PreferenceLabel() const;
    FString Label() const;
    TArray<FString> Controls() const;
    FString Format(const FString& Text) const;
    void Draw(AHUD* HUD, float X, float Y) const;
private:
    ECozyInputFamily Detected=ECozyInputFamily::Keyboard;
    int32 Override=0; // Auto, Xbox, PlayStation, keyboard, generic.
    TSharedPtr<IInputProcessor> Processor;
};
