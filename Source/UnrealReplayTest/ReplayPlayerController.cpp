// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayPlayerController.h"

#include "UnrealReplayTest.h"


AReplayPlayerController::AReplayPlayerController(const FObjectInitializer& ObjectInitializer) :	Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;

    GotoTimeDelegate.BindUObject(this, &AReplayPlayerController::OnGotoTimeDelegate);
}

/*
* Console Command : "demopause"
* @See UWorld::HandleDemoPauseCommand
*/
void AReplayPlayerController::PauseReplay()
{
    UWorld* World = GetWorld();
    check(World != nullptr);

    World->Exec(World, TEXT("DEMOPAUSE"));
}

bool AReplayPlayerController::IsPauseReplay()
{
    AWorldSettings* WorldSettings = GetWorldSettings();
    check(WorldSettings != nullptr);

    return WorldSettings->GetPauserPlayerState() != nullptr;
}

/*
* Console Command : "demostop"
* 
* @See UWorld::HandleDemoStopCommand
*/
void AReplayPlayerController::StopReplay()
{
    UGameInstance* GameInstance = GetGameInstance();
    check(GameInstance != nullptr);

    GameInstance->StopRecordingReplay();
}

float AReplayPlayerController::ReplayTotalTime() const
{
    UWorld* World = GetWorld();
    check(World != nullptr);

    UDemoNetDriver* DemoNetDriver = World->GetDemoNetDriver();
    if (DemoNetDriver != nullptr)
    {
        return DemoNetDriver->GetDemoTotalTime();
    }

    return 0.f;
}

float AReplayPlayerController::ReplayCurrentTime() const
{
    UWorld* World = GetWorld();
    check(World != nullptr);

    UDemoNetDriver* DemoNetDriver = World->GetDemoNetDriver();
    if (DemoNetDriver != nullptr)
    {
        return DemoNetDriver->GetDemoCurrentTime();
    }

    return 0.f;
}

/*
* Console Command : "demoscrub (GotoTime)"
* 
* @See UWorld::HandleDemoScrubCommand
*/
void AReplayPlayerController::ReplayGotoTime(const float GotoTime)
{
    UWorld* World = GetWorld();
    check(World != nullptr);

    UDemoNetDriver* DemoNetDriver = World->GetDemoNetDriver();
    if (DemoNetDriver != nullptr && DemoNetDriver->GetReplayStreamer().IsValid() && DemoNetDriver->ServerConnection != nullptr && DemoNetDriver->ServerConnection->OwningActor != nullptr)
    {
        APlayerController* PlayerController = Cast<APlayerController>(DemoNetDriver->ServerConnection->OwningActor);
        if (PlayerController != nullptr)
        {
            //GetWorldSettings()->SetPauserPlayerState(PlayerController->PlayerState);
            DemoNetDriver->GotoTimeInSeconds(GotoTime, GotoTimeDelegate);
        }
        else
        {
            TRACE(Error, "Invalid PlayerController");
        }
    }
    else
    {
        TRACE(Error, "Failed ReplayGotoTime");
    }
}

/*
* Console Command : "demospeed (ReplaySpeed)"
* 
* @See UWorld::HandleDemoSpeedCommand
*/
void AReplayPlayerController::SetReplaySpeed(float ReplaySpeed /*= 1.f*/)
{
    AWorldSettings* WorldSettings = GetWorldSettings();
    check(WorldSettings != nullptr);

    WorldSettings->DemoPlayTimeDilation = ReplaySpeed;
}

void AReplayPlayerController::OnGotoTimeDelegate(const bool bWasSuccessful)
{
    PostReplayGotoTime(bWasSuccessful);
}

