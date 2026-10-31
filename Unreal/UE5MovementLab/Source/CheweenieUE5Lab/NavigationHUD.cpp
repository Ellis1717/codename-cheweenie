#include "MartyCharacter.h"
#include "CozyAdventure.h"
#include "CozyNavigation.h"
#include "Neighbourhood.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"

// Clip the line, rather than clamping its endpoints and inventing roads along the map border.
static bool ClipLine(FVector2D& A,FVector2D& B,FVector2D Min,FVector2D Max)
{
    const FVector2D D=B-A;double Lo=0,Hi=1;
    auto Edge=[&](double P,double Q){if(FMath::Abs(P)<.00001)return Q>=0;const double T=Q/P;if(P<0)Lo=FMath::Max(Lo,T);else Hi=FMath::Min(Hi,T);return Lo<=Hi;};
    if(!Edge(-D.X,A.X-Min.X)||!Edge(D.X,Max.X-A.X)||!Edge(-D.Y,A.Y-Min.Y)||!Edge(D.Y,Max.Y-A.Y))return false;
    B=A+D*Hi;A+=D*Lo;return true;
}
void AMartyHUD::DrawNavigation(bool Full)
{
    const auto* M=GetWorld()->GetAuthGameMode<AMartyPlayMode>();const auto* PC=GetWorld()->GetFirstPlayerController();const APawn* P=PC?PC->GetPawn():nullptr;
    if(!M||!M->Navigation||!P||!M->Adventure->Progress)return;const auto* S=M->Adventure.Get();const auto* N=M->Navigation.Get();
    const float W=Canvas->SizeX,H=Canvas->SizeY;const auto Goal=N->Goal();
    if(Full&&!S->Feature(ECozyFeature::Map)){DrawText(TEXT("Map display is OFF."),FLinearColor::White,28,130);DrawText(TEXT("Enable it in Controls > Navigation."),FLinearColor(.7f,.85f,.8f),28,160);DrawText(TEXT("You can still choose a destination on the right."),FLinearColor(.7f,.85f,.8f),28,190);}
    if(S->Feature(ECozyFeature::Map))
    {
        const float Size=Full?FMath::Max(180.f,FMath::Min(W-525,H-255)):250.f;
        const float X=Full?28:W-278,Y=Full?105:135;
        DrawRect(FLinearColor(.025f,.065f,.075f,.96f),X,Y,Size,Size);
        FVector Center=P->GetActorLocation();float Span=M->Neighbourhood->Outside?7000:1200;
        const auto Stops=N->Landmarks();const auto& Roads=M->Neighbourhood->Streets();
        if(Full){FBox Bounds(ForceInit);Bounds+=Center;Bounds+=Goal.Position;for(const auto& L:Stops)if(L.Outdoors==M->Neighbourhood->Outside)Bounds+=L.Position;
            if(M->Neighbourhood->Outside){for(const auto& E:Roads){Bounds+=E.Key;Bounds+=E.Value;}}else{Bounds+=FVector(-500,-500,0);Bounds+=FVector(500,500,0);}Center=Bounds.GetCenter();const auto Ext=Bounds.GetSize();Span=FMath::Max(Ext.X,Ext.Y)*1.16f;Span=FMath::Max(Span,1200.f);}
        const float Scale=(Size-40)/Span;const FVector2D Min(X+10,Y+25),Max(X+Size-10,Y+Size-20);
        auto Point=[&](FVector V){return FVector2D(X+Size/2+(V.X-Center.X)*Scale,Y+Size/2+(V.Y-Center.Y)*Scale);};
        auto Line=[&](FVector A,FVector B,FLinearColor C,float Thickness){auto U=Point(A),V=Point(B);if(ClipLine(U,V,Min,Max))DrawLine(U.X,U.Y,V.X,V.Y,C,Thickness);};
        DrawText(Full?TEXT("AREA MAP - NORTH UP"):TEXT("LOCAL MAP - NORTH UP"),FLinearColor(.9f,.85f,.65f),X+12,Y+7);
        if(M->Neighbourhood->Outside){for(const auto& E:Roads)Line(E.Key,E.Value,FLinearColor(.3f,.4f,.44f),Full?3:5);}
        else{const FVector Corners[]={FVector(-480,-480,0),FVector(480,-480,0),FVector(480,480,0),FVector(-480,480,0)};for(int32 I=0;I<4;++I)Line(Corners[I],Corners[(I+1)%4],FLinearColor(.3f,.4f,.44f),3);}
        if(S->Feature(ECozyFeature::Route))for(int32 I=1;I<N->Route.Num();++I)Line(N->Route[I-1],N->Route[I],N->StreetRoute?FLinearColor(.3f,1,.7f):FLinearColor(.55f,.65f,.65f),2);
        for(int32 I=0;I<Stops.Num();++I){if(Stops[I].Outdoors!=M->Neighbourhood->Outside)continue;const auto V=Point(Stops[I].Position);if(V.X>Min.X&&V.X<Max.X&&V.Y>Min.Y&&V.Y<Max.Y){DrawRect(FLinearColor(.55f,.85f,1),V.X-3,V.Y-3,6,6);if(Full)DrawText(FString::FromInt(I+1),FLinearColor::White,V.X+5,V.Y);}}
        auto Arrow=[&](FVector2D Origin,float Yaw,float Length,FLinearColor C){const float Angle=FMath::DegreesToRadians(Yaw);const FVector2D D(FMath::Cos(Angle),FMath::Sin(Angle)),Side(-D.Y,D.X);const FVector2D Tip=Origin+D*Length;
            DrawLine(Origin.X,Origin.Y,Tip.X,Tip.Y,C,2);for(float Sign:{-1.f,1.f}){const auto End=Tip-D*7+Side*Sign*4;DrawLine(Tip.X,Tip.Y,End.X,End.Y,C,2);}};
        auto Me=Point(P->GetActorLocation());Me.X=FMath::Clamp(Me.X,Min.X+25,Max.X-25);Me.Y=FMath::Clamp(Me.Y,Min.Y+25,Max.Y-25);
        FVector CameraPosition;FRotator CameraRotation;PC->GetPlayerViewPoint(CameraPosition,CameraRotation);
        Arrow(Me,CameraRotation.Yaw,30,FLinearColor(.3f,.7f,1));Arrow(Me,P->GetActorRotation().Yaw,19,FLinearColor(1,.8f,.2f));DrawRect(FLinearColor(1,.8f,.2f),Me.X-3,Me.Y-3,6,6);
        if(S->Feature(ECozyFeature::Waypoint)){auto G=Point(Goal.Position);G.X=FMath::Clamp(G.X,Min.X+8,Max.X-8);G.Y=FMath::Clamp(G.Y,Min.Y+8,Max.Y-8);DrawText(TEXT("X"),FLinearColor(1,.6f,.35f),G.X-4,G.Y-5);}
        DrawText(TEXT("N"),FLinearColor::White,X+Size/2-4,Y+26);DrawText(FString::Printf(TEXT("%.0f m across"),Span/100),FLinearColor(.7f,.8f,.8f),X+12,Y+Size-18);
        if(Full){DrawText(TEXT("Gold: Marty | Blue: camera | X: destination"),FLinearColor::White,X,Y+Size+14);DrawText(TEXT("Mint: street guide; grey: direct bearing. Not obstacle avoidance."),FLinearColor(.7f,.85f,.8f),X,Y+Size+38);}
    }
    if(!Full&&S->Feature(ECozyFeature::Compass)){const float Bearing=UCozyNavigation::Bearing(P->GetActorForwardVector());DrawRect(FLinearColor(.025f,.065f,.075f,.94f),W-278,395,250,50);DrawText(FString::Printf(TEXT("Marty: %s %03.0f deg"),*UCozyNavigation::Cardinal(Bearing),Bearing),FLinearColor(1,.85f,.4f),W-266,403);DrawText(TEXT("Gold = body | Blue = camera"),FLinearColor(.6f,.8f,1),W-266,425);}
    if(!Full&&S->Feature(ECozyFeature::Location))DrawText(TEXT("You are at: ")+N->Location(),FLinearColor(.7f,.9f,1),28,193);
    if(!Full&&S->Feature(ECozyFeature::Waypoint)){DrawRect(FLinearColor(.025f,.065f,.075f,.94f),16,220,FMath::Min(W-32,960.f),48);DrawText(N->Guidance(),FLinearColor(1,.85f,.45f),28,231,nullptr,1.1f);}
}
