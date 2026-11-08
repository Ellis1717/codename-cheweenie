#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputModifiers.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "ControllerComfort.generated.h"

// Local input preferences are independent of adventure saves and follow map travel.
UCLASS()
class UControllerComfort : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    TArray<FString> Rows() const;
    void Adjust(int32 Row,int32 Direction);
    void ApplyPreset(bool Fast);
    void ResetDefaults(bool Persist=true);
    void ResetZoom();
    void Save();
    float FilterAxis(float Raw,bool Right) const;
    FVector2D ShapeLook(FVector2D Axis) const;
    void ClearLocalInput();
    float LeftDeadZone=.2f,RightDeadZone=.2f,LookSpeed=1.f;
    bool PreciseLook=true;
    int32 L3Mode=2; // 0 off, 1 hold, 2 toggle. A/Cross remains hold-to-hurry.
    float CameraDistances[6]={220,330,140,120,400,170};
private:
    void DeviceConnectionChanged(EInputDeviceConnectionState State,FPlatformUserId User,FInputDeviceId Device);
};

// Each mapping reads current preferences, avoiding a mapping rebuild mid-menu.
UCLASS()
class UCozyStickDeadZone : public UInputModifier
{
    GENERATED_BODY()
public:
    UPROPERTY() bool RightStick=false;
    virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput,FInputActionValue Value,float DeltaTime) override;
};
