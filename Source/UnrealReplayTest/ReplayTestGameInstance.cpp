// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayTestGameInstance.h"

#include "UnrealReplayTest.h"
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "Misc/NetworkVersion.h"


void UReplayTestGameInstance::Init()
{
	Super::Init();

	// create a ReplayStreamer for FindReplays() and DeleteReplay(..)
	EnumerateStreamsPtr = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();

	// Link FindReplays() delegate to function
	//OnEnumerateStreamsCompleteDelegate = FOnEnumerateStreamsComplete::CreateUObject(this, &UReplayTestGameInstance::OnEnumerateStreamsComplete);
    OnEnumerateStreamsCompleteDelegate = FEnumerateStreamsCallback::CreateUObject(this, &UReplayTestGameInstance::OnEnumerateStreamsComplete);

	// Link DeleteReplay() delegate to function
    //OnDeleteFinishedStreamCompleteDelegate = FOnDeleteFinishedStreamComplete::CreateUObject(this, &UReplayTestGameInstance::OnDeleteFinishedStreamComplete);
    OnDeleteFinishedStreamCompleteDelegate = FDeleteFinishedStreamCallback::CreateUObject(this, &UReplayTestGameInstance::OnDeleteFinishedStreamComplete);
}

void UReplayTestGameInstance::StartRecordingReplayFromBP(const FString& ReplayName, const FString& FriendlyName)
{
	StartRecordingReplay(ReplayName, FriendlyName);
}

void UReplayTestGameInstance::StopRecordingReplayFromBP()
{
	StopRecordingReplay();
}

void UReplayTestGameInstance::PlayReplayFromBP(const FString& ReplayName)
{
	PlayReplay(ReplayName);
}

void UReplayTestGameInstance::FindReplays()
{
    if (EnumerateStreamsPtr.Get())
    {
        TArray<FString> ExtraParms;

        EnumerateStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(), 0, FString(), ExtraParms, OnEnumerateStreamsCompleteDelegate);
    }
}

void UReplayTestGameInstance::RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName)
{
    // Get File Info  
    FNullReplayInfo Info;

    const FString DemoPath = FPaths::Combine(*FPaths::ProjectSavedDir(), TEXT("Demos/"));
    const FString StreamDirectory = FPaths::Combine(*DemoPath, *ReplayName);
    const FString StreamFullBaseFilename = FPaths::Combine(*StreamDirectory, *ReplayName);
    const FString InfoFilename = StreamFullBaseFilename + TEXT(".replayinfo");

    TUniquePtr<FArchive> InfoFileArchive(IFileManager::Get().CreateFileReader(*InfoFilename));

    if (InfoFileArchive.IsValid() && InfoFileArchive->TotalSize() != 0)
    {
        FString JsonString;
        *InfoFileArchive << JsonString;

        Info.FromJson(JsonString);
        Info.bIsValid = true;

        InfoFileArchive->Close();
    }

    // Set FriendlyName
    Info.FriendlyName = NewFriendlyReplayName;

    // Write File Info
    TUniquePtr<FArchive> ReplayInfoFileAr(IFileManager::Get().CreateFileWriter(*InfoFilename));

    if (ReplayInfoFileAr.IsValid())
    {
        FString JsonString = Info.ToJson();
        *ReplayInfoFileAr << JsonString;

        ReplayInfoFileAr->Close();
    }
}

void UReplayTestGameInstance::DeleteReplay(const FString& ReplayName)
{
    if (EnumerateStreamsPtr.Get())
    {
        //virtual void DeleteFinishedStream(const FString & StreamName, const FDeleteFinishedStreamCallback & Delegate) = 0;
        //virtual void DeleteFinishedStream(const FString & StreamName, const int32 UserIndex, const FDeleteFinishedStreamCallback & Delegate) = 0;

        EnumerateStreamsPtr.Get()->DeleteFinishedStream(ReplayName, OnDeleteFinishedStreamCompleteDelegate);
    }
}

//void UReplayTestGameInstance::OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo>& StreamInfos)
void UReplayTestGameInstance::OnEnumerateStreamsComplete(const FEnumerateStreamsResult& StreamResult)
{
    TArray<FS_ReplayInfo> AllReplays;

    for (FNetworkReplayStreamInfo StreamInfo : StreamResult.FoundStreams)
    {
        if (!StreamInfo.bIsLive)
        {
            AllReplays.Add(FS_ReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
        }
    }

    BP_OnFindReplaysComplete(AllReplays);
}

//void UReplayTestGameInstance::OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded)
void UReplayTestGameInstance::OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& DeleteResult)
{
    FindReplays();
}

