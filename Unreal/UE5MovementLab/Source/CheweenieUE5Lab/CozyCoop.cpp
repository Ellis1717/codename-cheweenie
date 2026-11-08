#include "CozyCoop.h"
#include "ControllerComfort.h"
#include "NaturalPetAudio.h"
#include "CozyHUDStyle.h"
#include "CozyModeHub.h"
#include "Engine/GameInstance.h"
#include "CozyAdventure.h"
#include "HalloweenCostume.h"
#include "MartyGait.h"
#include "DogLife.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Canvas.h"
#include "Engine/LocalPlayer.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

static FString PetLabel(int32 Id){return Id==1?TEXT("Lakitu"):Id==2?TEXT("Rio"):Id==0?TEXT("Marty"):Id==3?TEXT("Ratchet"):TEXT("Choosing pet");}
static void TellPet(ACozyCoopPet* Pet,const FString& Text){if(auto* PC=Cast<ACozyCoopController>(Pet->GetController()))PC->Notice(Text);}

static bool RatchetEarned(const UObject* Context){const FString Slot=(FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubSmoke"))||FParse::Param(FCommandLine::Get(),TEXT("CheweenieHubArchiveSmoke")))?CozyHubProfileSlot():(FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetSmoke"))||FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetLan")))?TEXT("CheweenieRatchetAcceptance"):FParse::Param(FCommandLine::Get(),TEXT("CheweenieCoopSmoke"))?TEXT("CheweenieCoopAcceptance"):TEXT("CheweenieAdventureV1");const auto* Save=Cast<UCozyAdventureSave>(UGameplayStatics::LoadGameFromSlot(Slot,0));return Save&&Save->Version==1&&(Save->RatchetUnlocked||Save->Complete);}
void ACozyCoopState::BeginPlay(){Super::BeginPlay();if(HasAuthority())RatchetUnlocked=RatchetEarned(this);PrepareCheweenieRoom(GetWorld(),true);}
void ACozyCoopState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{Super::GetLifetimeReplicatedProps(OutLifetimeProps);DOREPLIFETIME(ACozyCoopState,RatchetUnlocked);}
ACozyCoopMode::ACozyCoopMode()
{
    DefaultPawnClass=ACozyCoopPet::StaticClass();PlayerControllerClass=ACozyCoopController::StaticClass();
    GameStateClass=ACozyCoopState::StaticClass();HUDClass=ACozyCoopHUD::StaticClass();
}
void ACozyCoopMode::BeginPlay(){Super::BeginPlay();GetWorld()->SpawnActor<ACozyCoopToy>(FVector(-120,-160,12),FRotator::ZeroRotator);}
void ACozyCoopMode::PreLogin(const FString& Options,const FString& Address,const FUniqueNetIdRepl& Id,FString& Error)
{
    Super::PreLogin(Options,Address,Id,Error);if(Error.IsEmpty()&&GetNumPlayers()>=2)Error=TEXT("This two-player room is full. Try again when a friend leaves.");
}
void ACozyCoopMode::RestartPlayer(AController* Player)
{
    if(!Player||Player->GetPawn())return;
    int32 Choice=0;
    for(;Choice<3;++Choice){bool Used=false;for(TActorIterator<ACozyCoopPet> It(GetWorld());It;++It)Used|=It->PetId==Choice;if(!Used)break;}
    if(Choice>=3)return;
    if(GetNetMode()==NM_Standalone&&GetWorld()->URL.HasOption(TEXT("RatchetVisit"))&&RatchetEarned(this))Choice=3;
    FActorSpawnParameters Params;Params.Owner=Player;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
    auto* Pet=GetWorld()->SpawnActor<ACozyCoopPet>(DefaultPawnClass,FVector(-240+(Choice==3?0:Choice)*120,-200+(Choice==3?0:Choice)*80,60),FRotator::ZeroRotator,Params);
    if(Pet){Pet->PetId=Choice;Pet->ApplyPet();Player->Possess(Pet);}
}

ACozyCoopPet::ACozyCoopPet()
{
    bReplicates=true;SetReplicateMovement(true);SetNetUpdateFrequency(20);SetMinNetUpdateFrequency(2);
    // CharacterMovement smooths the inherited mesh on simulated proxies. Keep the actual
    // pet beneath it so a 20 Hz capsule update does not step the rendered fur/body.
    Skin->SetupAttachment(GetMesh());
    if(auto* Static=Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("RetainedMarty"))))Static->SetupAttachment(GetMesh());
    // Only transforms, velocity and small gameplay fields cross the network; fur poses stay local.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> L(TEXT("/Game/VisualLab/Pets/Lakitu.Lakitu"));LakituMesh=L.Object;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> R(TEXT("/Game/VisualLab/Pets/Rio.Rio"));RioMesh=R.Object;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> T(TEXT("/Game/Adventure/Ratchet/Ratchet_Tribute.Ratchet_Tribute"));RatchetMesh=T.Object;
    static ConstructorHelpers::FObjectFinder<USoundBase> B(TEXT("/Game/Adventure/Audio/Bark.Bark"));
    static ConstructorHelpers::FObjectFinder<USoundBase> K(TEXT("/Game/Adventure/Audio/Lakitu.Lakitu"));
    static ConstructorHelpers::FObjectFinder<USoundBase> S(TEXT("/Game/Adventure/Audio/Rio.Rio"));Voices={B.Object,K.Object,S.Object};
    LoadNaturalBarks(BarkTakes);
    if(auto* Voice=NaturalCatVoice(TEXT("Lakitu")))Voices[1]=Voice;
    if(auto* Voice=NaturalCatVoice(TEXT("Rio")))Voices[2]=Voice;
}
void ACozyCoopPet::BeginPlay()
{
    Super::BeginPlay();Life->SetComponentTickEnabled(false);ApplyPet();
}
void ACozyCoopPet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);DOREPLIFETIME(ACozyCoopPet,PetId);DOREPLIFETIME(ACozyCoopPet,WearingCostume);DOREPLIFETIME(ACozyCoopPet,VoiceCount);DOREPLIFETIME(ACozyCoopPet,LastVoicePet);DOREPLIFETIME(ACozyCoopPet,AcceptanceStage);
}
FString ACozyCoopPet::PetName() const{return PetLabel(PetId);}
void ACozyCoopPet::ApplyPet()
{
    auto* Static=Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("RetainedMarty")));
    const bool Dog=PetId==0;
    Skin->SetVisibility(Dog);Gait->SetComponentTickEnabled(Dog&&GetNetMode()!=NM_DedicatedServer);
    if(Static){Static->SetVisibility(PetId>0);if(PetId>0){Static->SetStaticMesh(PetId==1?LakituMesh:PetId==2?RioMesh:RatchetMesh);Static->SetRelativeScale3D(FVector(PetId==3?1.f:.32f));}}
    Costume->SetOutfit(Dog?static_cast<USceneComponent*>(Skin.Get()):Static,PetId,WearingCostume&&PetId<3);
    if(Dog&&Gait->Ready)Gait->ResetContacts();
}
bool ACozyCoopPet::PetAvailable(int32 Id) const
{
    if(Id<0||Id>3)return false;if(Id==3){const auto* State=GetWorld()->GetGameState<ACozyCoopState>();if(!State||!State->RatchetUnlocked||!RatchetMesh)return false;}for(TActorIterator<ACozyCoopPet> It(GetWorld());It;++It)if(*It!=this&&It->PetId==Id)return false;return true;
}
void ACozyCoopPet::ServerAcceptanceStage_Implementation(int32 Value)
{
    if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieCoopSmoke"))&&Value>=0&&Value<=14){AcceptanceStage=Value;ForceNetUpdate();}
}
void ACozyCoopPet::ServerChoosePet_Implementation(int32 Id)
{
    if(!PetAvailable(Id)){TellPet(this,TEXT("That pet is locked or already playing. Choose another friend."));return;}
    PetId=Id;ApplyPet();ForceNetUpdate();TellPet(this,FString(TEXT("You are now "))+PetName()+TEXT("!"));
}
void ACozyCoopPet::Tick(float Delta)
{
    Super::Tick(Delta);
    const int32 Policy=IsLocallyControlled()?0:1;
    if(PosePolicy!=Policy){PosePolicy=Policy;Gait->SetComponentTickInterval(Policy==0?0.f:.05f);if(Policy==0)RecenterCoop();}
    if(IsLocallyControlled())if(auto* PC=Cast<APlayerController>(Controller))if(PC->PlayerCameraManager){PC->PlayerCameraManager->ViewPitchMin=-55;PC->PlayerCameraManager->ViewPitchMax=20;}
}
void ACozyCoopPet::EndPlay(const EEndPlayReason::Type Reason)
{
    if(HasAuthority())for(TActorIterator<ACozyCoopToy> It(GetWorld());It;++It)if(It->Carrier==this)It->Release();
    if(auto* PC=Cast<APlayerController>(Controller))if(auto* S=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))if(CoopMapping)S->RemoveMappingContext(CoopMapping);
    Super::EndPlay(Reason);
}
void ACozyCoopPet::ServerHurry_Implementation(bool Active){GetCharacterMovement()->MaxWalkSpeed=Active?230:100;}
void ACozyCoopPet::ReturnHome(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen){GetGameInstance()->GetSubsystem<UCozyModeHub>()->Navigate(1);return;}ClearControllerInput();if(HasAuthority())ServerHome_Implementation();else if(IsLocallyControlled())ServerHome();}
void ACozyCoopPet::ServerHome_Implementation()
{
    ServerToy(true);GetCharacterMovement()->StopMovementImmediately();const int32 Slot=PetId==3?0:FMath::Max(0,PetId);TeleportTo(FVector(-240+Slot*120,-200+Slot*80,60),FRotator::ZeroRotator);ForceNetUpdate();
}
void ACozyCoopPet::ServerVoice_Implementation()
{
    const double Now=GetWorld()->GetTimeSeconds();if(Now<NextVoice||PetId<0||PetId>3)return;NextVoice=Now+1.2;++VoiceCount;LastVoicePet=PetId;PlayVoice(PetId==3?2:PetId);
}
void ACozyCoopPet::PlayVoice_Implementation(int32 Id)
{
    if(GetNetMode()==NM_DedicatedServer||!Voices.IsValidIndex(Id))return;
    USoundBase* Voice=Id==0?ChooseNaturalBark(BarkTakes,LastBarkTake):nullptr;
    if(!Voice)Voice=Voices[Id];
    if(Voice)UGameplayStatics::PlaySoundAtLocation(this,Voice,GetActorLocation(),.6f);
}
void ACozyCoopPet::ServerToy_Implementation(bool DropOnly)
{
    for(TActorIterator<ACozyCoopToy> It(GetWorld());It;++It)
    {
        if(It->Carrier==this){It->Release();return;}
        if(!DropOnly&&!It->Carrier&&FVector::Dist(GetActorLocation(),It->GetActorLocation())<85){It->Carrier=this;It->SetReplicateMovement(false);It->SyncCarrier();It->ForceNetUpdate();return;}
    }
}
void ACozyCoopPet::MoveCoop(const FInputActionValue& V)
{
    if(!Controller)return;if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)return;
    const FVector2D A=V.Get<FVector2D>().GetClampedToMaxSize(1);const FRotator Heading(0,Controller->GetControlRotation().Yaw,0);
    AddMovementInput(Heading.Vector(),A.Y);AddMovementInput(FRotationMatrix(Heading).GetUnitAxis(EAxis::Y),A.X);
}
void ACozyCoopPet::MouseCoop(const FInputActionValue& V){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)return;auto A=V.Get<FVector2D>();AddControllerYawInput(A.X);AddControllerPitchInput(A.Y);}
void ACozyCoopPet::StickCoop(const FInputActionValue& V){MouseCoop(FInputActionValue(GetGameInstance()->GetSubsystem<UControllerComfort>()->ShapeLook(V.Get<FVector2D>())*(80*GetWorld()->GetDeltaSeconds())));}
void ACozyCoopPet::ApplyControllerHurry(bool Active){GetCharacterMovement()->MaxWalkSpeed=Active?230:100;ServerHurry(Active);}
void ACozyCoopPet::HurryOn(){if(BlockHurryAfterMenu)return;if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen){HubConfirm();return;}SetControllerHurryHeld(true);}
void ACozyCoopPet::HurryOff(){BlockHurryAfterMenu=false;SetControllerHurryHeld(false);}
void ACozyCoopPet::NextPet(){if(auto* PC=Cast<ACozyCoopController>(Controller)){if(PC->MenuOpen){GetGameInstance()->GetSubsystem<UCozyModeHub>()->Adjust(1);return;}if(PC->IsInputKeyDown(EKeys::Gamepad_RightShoulder)){ZoomCamera(30);return;}}for(int32 I=1;I<4;++I){const int32 Id=(PetId+I)%4;if(PetAvailable(Id)){ServerChoosePet(Id);return;}}}
void ACozyCoopPet::PreviousPet(){if(auto* PC=Cast<ACozyCoopController>(Controller)){if(PC->MenuOpen){GetGameInstance()->GetSubsystem<UCozyModeHub>()->Adjust(-1);return;}if(PC->IsInputKeyDown(EKeys::Gamepad_RightShoulder)){ZoomCamera(-30);return;}}for(int32 I=1;I<4;++I){const int32 Id=(PetId+4-I)%4;if(PetAvailable(Id)){ServerChoosePet(Id);return;}}}
void ACozyCoopPet::Marty(){ServerChoosePet(0);}void ACozyCoopPet::Lakitu(){ServerChoosePet(1);}void ACozyCoopPet::Rio(){ServerChoosePet(2);}
void ACozyCoopPet::Ratchet(){ServerChoosePet(3);}
void ACozyCoopPet::ReturnPatrol(){if(GetNetMode()==NM_Standalone)UGameplayStatics::OpenLevel(this,TEXT("/Game/VisualLab/Comparison"),true,TEXT("game=/Script/CheweenieUE5Lab.MartyPlayMode"));}
void ACozyCoopPet::JumpCoop(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)return;Jump();}void ACozyCoopPet::Voice(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)return;ServerVoice();}void ACozyCoopPet::UseToy(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)return;ServerToy(false);}void ACozyCoopPet::DropToy(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)return;ServerToy(true);}
void ACozyCoopPet::RecenterCoop(){if(Controller)Controller->SetControlRotation(FRotator(CameraPreset==4?-50:CameraPreset==5?-5:-12,GetActorRotation().Yaw,0));}
void ACozyCoopPet::ToggleCostume(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen){GetGameInstance()->GetSubsystem<UCozyModeHub>()->Navigate(-1);return;}ServerToggleCostume();}
void ACozyCoopPet::ServerToggleCostume_Implementation(){if(PetId==3){TellPet(this,TEXT("Ratchet is keeping his own cozy coat today."));return;}WearingCostume=!WearingCostume;ApplyPet();ForceNetUpdate();}
void ACozyCoopPet::ToggleMenu(){HurryOff();GetCharacterMovement()->StopMovementImmediately();if(auto* PC=Cast<ACozyCoopController>(Controller))PC->SetHubMenu(!PC->MenuOpen);}
void ACozyCoopPet::SetupPlayerInputComponent(UInputComponent* Input)
{
    // Do not install the solo menu/action bindings on a network pawn.
    auto* E=CastChecked<UEnhancedInputComponent>(Input);CoopMapping=NewObject<UInputMappingContext>(this);
    auto Action=[&](EInputActionValueType T){auto* A=NewObject<UInputAction>(this);A->ValueType=T;CoopActions.Add(A);return A;};
    auto Axis=[&](UInputAction* A,FKey K,bool Y,bool Minus){auto& M=CoopMapping->MapKey(A,K);
        if(Minus)M.Modifiers.Add(NewObject<UInputModifierNegate>(CoopMapping));if(Y){auto* S=NewObject<UInputModifierSwizzleAxis>(CoopMapping);S->Order=EInputAxisSwizzle::YXZ;M.Modifiers.Add(S);}};
    auto StickMap=[&](UInputAction* A,FKey K,bool Right){auto& M=CoopMapping->MapKey(A,K);auto* Mod=NewObject<UCozyStickDeadZone>(CoopMapping);Mod->RightStick=Right;M.Modifiers.Add(Mod);};
    auto* Move=Action(EInputActionValueType::Axis2D);Axis(Move,EKeys::W,true,false);Axis(Move,EKeys::S,true,true);Axis(Move,EKeys::A,false,true);Axis(Move,EKeys::D,false,false);StickMap(Move,EKeys::Gamepad_Left2D,false);E->BindAction(Move,ETriggerEvent::Triggered,this,&ACozyCoopPet::MoveCoop);
    auto* Mouse=Action(EInputActionValueType::Axis2D);Axis(Mouse,EKeys::MouseX,false,false);Axis(Mouse,EKeys::MouseY,true,false);E->BindAction(Mouse,ETriggerEvent::Triggered,this,&ACozyCoopPet::MouseCoop);
    auto* Stick=Action(EInputActionValueType::Axis2D);StickMap(Stick,EKeys::Gamepad_Right2D,true);E->BindAction(Stick,ETriggerEvent::Triggered,this,&ACozyCoopPet::StickCoop);
    auto Button=[&](FKey K,FKey Pad,auto Callback){auto* A=Action(EInputActionValueType::Boolean);if(K.IsValid())CoopMapping->MapKey(A,K);if(Pad.IsValid())CoopMapping->MapKey(A,Pad);E->BindAction(A,ETriggerEvent::Started,this,Callback);return A;};
    auto* Run=Button(EKeys::LeftShift,EKeys::Gamepad_FaceButton_Bottom,&ACozyCoopPet::HurryOn);E->BindAction(Run,ETriggerEvent::Completed,this,&ACozyCoopPet::HurryOff);E->BindAction(Run,ETriggerEvent::Canceled,this,&ACozyCoopPet::HurryOff);
    auto* HurryClick=Button(EKeys::Invalid,EKeys::Gamepad_LeftThumbstick,&ACozyCoopPet::ControllerHurryPress);E->BindAction(HurryClick,ETriggerEvent::Completed,this,&ACozyCoopPet::ControllerHurryRelease);E->BindAction(HurryClick,ETriggerEvent::Canceled,this,&ACozyCoopPet::ControllerHurryRelease);
    auto* Hop=Button(EKeys::SpaceBar,EKeys::Gamepad_FaceButton_Top,&ACozyCoopPet::JumpCoop);E->BindAction(Hop,ETriggerEvent::Completed,this,&ACozyCoopPet::StopJumping);
    Button(EKeys::V,EKeys::Gamepad_RightThumbstick,&ACozyCoopPet::CycleCamera);Button(EKeys::R,EKeys::Gamepad_RightShoulder,&ACozyCoopPet::RecenterCoop);
    SetupCameraZoom(E,CoopMapping);
    Button(EKeys::H,EKeys::Gamepad_DPad_Down,&ACozyCoopPet::ReturnHome);Button(EKeys::B,EKeys::Gamepad_LeftShoulder,&ACozyCoopPet::Voice);Button(EKeys::E,EKeys::Gamepad_FaceButton_Left,&ACozyCoopPet::UseToy);Button(EKeys::Q,EKeys::Gamepad_LeftTrigger,&ACozyCoopPet::DropToy);
    Button(EKeys::Enter,EKeys::Invalid,&ACozyCoopPet::HubConfirm);Button(EKeys::BackSpace,EKeys::Gamepad_FaceButton_Right,&ACozyCoopPet::HubBack);Button(EKeys::Up,EKeys::Invalid,&ACozyCoopPet::HubUp);Button(EKeys::Down,EKeys::Invalid,&ACozyCoopPet::HubDown);Button(EKeys::Left,EKeys::Invalid,&ACozyCoopPet::HubLeft);Button(EKeys::Right,EKeys::Invalid,&ACozyCoopPet::HubRight);
    Button(EKeys::Four,EKeys::Invalid,&ACozyCoopPet::Ratchet);Button(EKeys::F9,EKeys::Gamepad_Special_Left,&ACozyCoopPet::ReturnPatrol);
    Button(EKeys::T,EKeys::Gamepad_DPad_Up,&ACozyCoopPet::ToggleCostume);
    Button(EKeys::One,EKeys::Invalid,&ACozyCoopPet::Marty);Button(EKeys::Two,EKeys::Invalid,&ACozyCoopPet::Lakitu);Button(EKeys::Three,EKeys::Invalid,&ACozyCoopPet::Rio);
    Button(EKeys::RightBracket,EKeys::Gamepad_DPad_Right,&ACozyCoopPet::NextPet);Button(EKeys::LeftBracket,EKeys::Gamepad_DPad_Left,&ACozyCoopPet::PreviousPet);Button(EKeys::Escape,EKeys::Gamepad_Special_Right,&ACozyCoopPet::ToggleMenu);
    if(auto* PC=Cast<APlayerController>(Controller))if(auto* S=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))S->AddMappingContext(CoopMapping,0);
}

ACozyCoopToy::ACozyCoopToy()
{
    bReplicates=true;SetReplicateMovement(true);SetNetUpdateFrequency(10);SetMinNetUpdateFrequency(2);PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.bStartWithTickEnabled=false;
    Ball=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SharedBall"));SetRootComponent(Ball);Ball->SetCollisionEnabled(ECollisionEnabled::NoCollision);Ball->SetRelativeScale3D(FVector(.13f));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> M(TEXT("/Engine/BasicShapes/Sphere.Sphere"));Ball->SetStaticMesh(M.Object);
}
void ACozyCoopToy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const{Super::GetLifetimeReplicatedProps(OutLifetimeProps);DOREPLIFETIME(ACozyCoopToy,Carrier);}
void ACozyCoopToy::SyncCarrier(){SetActorTickEnabled(Carrier!=nullptr);if(Carrier)SetActorLocation(Carrier->GetActorLocation()+Carrier->GetActorForwardVector()*34+FVector(0,0,-8));}
void ACozyCoopToy::Tick(float Delta){Super::Tick(Delta);if(Carrier)SyncCarrier();else if(HasAuthority())Release();}
void ACozyCoopToy::Release()
{
    FVector Spot=GetActorLocation();if(Carrier)Spot=Carrier->GetActorLocation()+Carrier->GetActorForwardVector()*55;
    FHitResult Hit;FCollisionQueryParams Q(SCENE_QUERY_STAT(CoopDrop),false,Carrier);Q.AddIgnoredActor(this);
    if(GetWorld()->LineTraceSingleByChannel(Hit,Spot+FVector(0,0,50),Spot-FVector(0,0,150),ECC_WorldStatic,Q))Spot=Hit.ImpactPoint+FVector(0,0,8);
    Carrier=nullptr;SetReplicateMovement(true);SetActorLocation(Spot);SetActorTickEnabled(false);ForceNetUpdate();
}

ACozyCoopController::ACozyCoopController(){PrimaryActorTick.bCanEverTick=true;}
void ACozyCoopController::BeginPlay(){Super::BeginPlay();Started=FPlatformTime::Seconds();if(IsLocalController()){SetInputMode(FInputModeGameOnly());bShowMouseCursor=false;GetGameInstance()->GetSubsystem<UCozyModeHub>()->RoomReady();}}
void ACozyCoopController::Notice_Implementation(const FString& Text){Message=Text;}
void ACozyCoopController::Tick(float Delta){Super::Tick(Delta);if(IsLocalController()){if(FParse::Param(FCommandLine::Get(),TEXT("CheweenieRatchetSmoke")))TickRatchetAcceptance(Delta);else TickAcceptance(Delta);}}

void ACozyCoopHUD::DrawHUD()
{
    Super::DrawHUD();if(!Canvas)return;auto* PC=Cast<ACozyCoopController>(PlayerOwner);auto* Pet=PC?Cast<ACozyCoopPet>(PC->GetPawn()):nullptr;
    if(PC&&PC->MenuOpen){DrawHub();return;}
    const auto* Hub=GetGameInstance()->GetSubsystem<UCozyModeHub>();
    if(Hub->OverlayVisible(ECozyOverlay::RoomStatus)||(Hub->OverlayVisible(ECozyOverlay::Hints)))CozyDrawRect(this,FLinearColor(.025f,.055f,.07f,.92f),16,16,850,180);
    if(Hub->OverlayVisible(ECozyOverlay::RoomStatus))
    {
    CozyDrawText(this,GetNetMode()==NM_Standalone?TEXT("Codename .... Cheweenie | Ratchet memory walk"):TEXT("Codename .... Cheweenie | LAN Cozy Crew"),FLinearColor(1,.86f,.5f),28,28,nullptr,1.3f);
    CozyDrawText(this,Pet?FString(TEXT("You: "))+Pet->PetName()+(Pet->GetCharacterMovement()->MaxWalkSpeed>150?TEXT(" | Hurry ON"):TEXT("")):TEXT("Joining the crew..."),FLinearColor::White,28,58);
    }
    if(Hub->OverlayVisible(ECozyOverlay::Hints))
    {
    CozyDrawText(this,FString(TEXT("WASD / LS: move | mouse / RS: look | Shift / A-Cross: hold hurry | L3: "))+(GetGameInstance()->GetSubsystem<UControllerComfort>()->L3Mode==2?TEXT("toggle"):GetGameInstance()->GetSubsystem<UControllerComfort>()->L3Mode==1?TEXT("hold"):TEXT("off")),FLinearColor::White,28,83);
    CozyDrawText(this,TEXT("Space / Y-Triangle: jump | B / LB-L1: voice | E / X-Square: carry | Q / LT: drop"),FLinearColor::White,28,105);
    CozyDrawText(this,TEXT("1 Marty | 2 Lakitu | 3 Rio | D-pad left/right: change pet | V / R3: camera"),FLinearColor::White,28,127);
    const auto* State=GetWorld()->GetGameState<ACozyCoopState>();CozyDrawText(this,State&&State->RatchetUnlocked?TEXT("4 Ratchet unlocked | T / D-pad Up: costume | F9 / View: return (offline)"):TEXT("Ratchet locked: host finishes solo Cozy Patrol | T / D-pad Up: costume"),FLinearColor::White,28,149);
    }
    if(Hub->OverlayVisible(ECozyOverlay::Hints))CozyDrawText(this,TEXT("RB-R1: recenter | Hold RB + D-pad left/right: zoom | Menu-Options: menu"),FLinearColor::White,28,171);
    if(PC&&PC->MenuOpen){CozyDrawText(this,TEXT("Controls open - friends keep playing. Esc / Options: close"),FLinearColor::White,28,195);CozyDrawText(this,TEXT("H: return home | R / RB-R1: recenter | Alt+F4: leave game"),FLinearColor::White,28,225);CozyDrawText(this,TEXT("LAN room: shared toy and voices. Solo quests/saves remain separate."),FLinearColor::White,28,255);}
    if(Hub->OverlayVisible(ECozyOverlay::Messages)&&PC&&!PC->Message.IsEmpty())CozyDrawText(this,PC->Message,FLinearColor(1,.85f,.45f),28,Canvas->SizeY-50);
    if(Hub->OverlayVisible(ECozyOverlay::PetNames))for(TActorIterator<ACozyCoopPet> It(GetWorld());It;++It)if(*It!=Pet){const FVector P=Project(It->GetActorLocation()+FVector(0,0,50));if(P.Z>0)CozyDrawText(this,It->PetName(),FLinearColor(1,.9f,.6f),P.X-20,P.Y);}
}

void ACozyCoopPet::HubConfirm(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen){BlockHurryAfterMenu=PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom);GetGameInstance()->GetSubsystem<UCozyModeHub>()->Confirm();}}
void ACozyCoopPet::HubBack(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)GetGameInstance()->GetSubsystem<UCozyModeHub>()->Back();}
void ACozyCoopPet::HubUp(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)GetGameInstance()->GetSubsystem<UCozyModeHub>()->Navigate(-1);}
void ACozyCoopPet::HubDown(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)GetGameInstance()->GetSubsystem<UCozyModeHub>()->Navigate(1);}
void ACozyCoopPet::HubLeft(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)GetGameInstance()->GetSubsystem<UCozyModeHub>()->Adjust(-1);}
void ACozyCoopPet::HubRight(){if(auto* PC=Cast<ACozyCoopController>(Controller))if(PC->MenuOpen)GetGameInstance()->GetSubsystem<UCozyModeHub>()->Adjust(1);}
void ACozyCoopController::SetHubMenu(bool Open){MenuOpen=Open;if(auto* P=Cast<ACozyCoopPet>(GetPawn())){P->ClearControllerInput();if(Open)P->GetCharacterMovement()->StopMovementImmediately();}bShowMouseCursor=Open;bEnableClickEvents=Open;if(Open){GetGameInstance()->GetSubsystem<UCozyModeHub>()->Open();SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));}else SetInputMode(FInputModeGameOnly());}
