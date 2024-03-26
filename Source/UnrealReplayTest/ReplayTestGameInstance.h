// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkReplayStreaming.h"

#include "ReplayTestGameInstance.generated.h"


USTRUCT(BlueprintType)  
struct FS_ReplayInfo  
{  
	GENERATED_USTRUCT_BODY()  
    
	UPROPERTY(BlueprintReadOnly)  
	FString ReplayName;  
    
	UPROPERTY(BlueprintReadOnly)  
	FString FriendlyName;  
    
	UPROPERTY(BlueprintReadOnly)  
	FDateTime Timestamp;  
    
	UPROPERTY(BlueprintReadOnly)  
	int32 LengthInMS;  
    
	UPROPERTY(BlueprintReadOnly)  
	bool bIsValid;  
    
	FS_ReplayInfo(FString& NewName, FString& NewFriendlyName, FDateTime NewTimestamp, int32 NewLengthInMS)  
	{  
		ReplayName = NewName;  
		FriendlyName = NewFriendlyName;  
		Timestamp = NewTimestamp;  
		LengthInMS = NewLengthInMS;  
		bIsValid = true;  
	}  

	FS_ReplayInfo()  
	{  
		ReplayName = "Replay";  
		FriendlyName = "Replay";  
		Timestamp = FDateTime::MinValue();  
		LengthInMS = 0;  
		bIsValid = false;  
	}  
};

/**
 * 
 */
UCLASS()
class UNREALREPLAYTEST_API UReplayTestGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
    /** virtual function to allow custom GameInstances an opportunity to set up what it needs */
    virtual void Init() override;


    /** Start recording a replay from blueprint. ReplayName = Name of file on disk, FriendlyName = Name of replay in UI */   
    UFUNCTION(BlueprintCallable, Category = "Replays")   
    void StartRecordingReplayFromBP(const FString& ReplayName, const FString& FriendlyName);   
  
    /** Start recording a running replay and save it, from blueprint. */   
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StopRecordingReplayFromBP();   
  
    /** Start playback for a previously recorded Replay, from blueprint */   
    UFUNCTION(BlueprintCallable, Category = "Replays")   
    void PlayReplayFromBP(const FString& ReplayName);   
  
    /** Start looking for/finding replays on the hard drive */  
    UFUNCTION(BlueprintCallable, Category = "Replays")  
    void FindReplays(); 
  
    /** Apply a new custom name to the replay (for UI only) */  
    UFUNCTION(BlueprintCallable, Category = "Replays")  
    void RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName);   
  
    /** Delete a previously recorded replay */
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void DeleteReplay(const FString& ReplayName);


protected:  
	UFUNCTION(BlueprintImplementableEvent, Category = "Replays")  
	void BP_OnFindReplaysComplete(const TArray<FS_ReplayInfo>& AllReplays);


private:
    // for FindReplays()   
    TSharedPtr<INetworkReplayStreamer> EnumerateStreamsPtr;
    //FOnEnumerateStreamsComplete OnEnumerateStreamsCompleteDelegate;
    FEnumerateStreamsCallback OnEnumerateStreamsCompleteDelegate;

    //void OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo>& StreamInfos);
    void OnEnumerateStreamsComplete(const FEnumerateStreamsResult& StreamResult);
   

    // for DeleteReplays(..)  
    //FOnDeleteFinishedStreamComplete OnDeleteFinishedStreamCompleteDelegate;
    FDeleteFinishedStreamCallback OnDeleteFinishedStreamCompleteDelegate;

    //void OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded);
    void OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult& DeleteResult);


    //
    FNetworkReplayVersion CurrentReplayVersion;
};

