#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Neighbourhood.generated.h"
class UMaterialInstanceDynamic;
class AStaticMeshActor;
class AMartyCharacter;
class UInstancedStaticMeshComponent;
class UDirectionalLightComponent;

struct FNeighbourDog
{
    FString Name, House;
    FVector Position;
    FLinearColor Coat=FLinearColor(.5f,.25f,.1f);
    bool Met=false;
};
struct FCozyStreet { FVector A,B; FString Name; };

// The original room stays independent. Exterior geometry and private maps belong to this component.
UCLASS(ClassGroup=(Cheweenie),meta=(BlueprintSpawnableComponent))
class UCheweenieNeighbourhood : public UActorComponent
{
    GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    bool Interact(AMartyCharacter* Pawn);
    FString Prompt(const AMartyCharacter* Pawn) const;
    void GoOutside(AMartyCharacter* Pawn);
    void GoInside(AMartyCharacter* Pawn);
    void SetOutdoorLighting(bool Active);
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) bool Outside=false;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) bool PrivateMapLoaded=false;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 Houses=0;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 RoadSegments=0;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly) int32 MetDogs=0;
    FVector Entrance=FVector(1000000,0,28);
    FVector Park, School;
    TArray<FNeighbourDog> Dogs;
    const TArray<TPair<FVector,FVector>>& Streets() const { return NavigationEdges; }
    TArray<TPair<FVector,FString>> StreetLabels;
    TArray<FCozyStreet> NamedStreets;
private:
    FVector Origin=FVector(1000000,0,0);
    FVector IndoorDoor=FVector(-425,-200,28);
    TArray<FVector> Tour;
    TArray<TPair<FVector,FVector>> RoadEdges;
    TArray<TPair<FVector,FVector>> NavigationEdges;
    int32 NextTour=0;
    UPROPERTY() TArray<TObjectPtr<UMaterialInstanceDynamic>> Palette;
    UPROPERTY() TMap<int32,TObjectPtr<UInstancedStaticMeshComponent>> Paths;
    UPROPERTY() TArray<TObjectPtr<UDirectionalLightComponent>> OutdoorFill;
    TArray<TPair<TWeakObjectPtr<UDirectionalLightComponent>,float>> OriginalSuns;
    AStaticMeshActor* Shape(const FString& Name,FVector P,FVector Size,int32 Color,bool Solid=true,const TCHAR* Mesh=TEXT("/Engine/BasicShapes/Cube.Cube"),float Yaw=0);
    void Sign(const FString& Text,FVector P,float Size=34,float Yaw=180);
    void Path(FVector A,FVector B,float Width,int32 Color);
    void House(FVector P,int32 Slot,const FString& Label);
    void Home();
    void Dog(const FNeighbourDog& Definition,int32 Index);
    void Build();
    bool LoadMap();
    void LoadDogs();
    void Message(const FString& Text);
};
