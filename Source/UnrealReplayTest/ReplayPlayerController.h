// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Engine/DemoNetDriver.h"

#include "ReplayPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class UNREALREPLAYTEST_API AReplayPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	/** we must set some Pause-Behavior values in the ctor */
	AReplayPlayerController(const FObjectInitializer& ObjectInitializer);


	/*
	* Replay Pause Toggle.
	*/
	UFUNCTION(BlueprintCallable)
	void PauseReplay();

	/*
	* Return Replay Pause State
	* @return Pause State - true : Pause, false : UnPause
	*/
	UFUNCTION(BlueprintPure)
	bool IsPauseReplay();

	/*
	* Stop Replay
	*/
	UFUNCTION(BlueprintCallable)
	void StopReplay();

	/*
	* Total Time (second) of the currently playing Replay
	*/
	UFUNCTION(BlueprintPure)
	float ReplayTotalTime() const;

	/*
	* Current Time (second) of the currently playing Replay
	*/
	UFUNCTION(BlueprintPure)
	float ReplayCurrentTime() const;

	/*
	* Jump to the Specified Time (second) In the Replay
	*/
	UFUNCTION(BlueprintCallable)
	void ReplayGotoTime(const float GotoTime);

	UFUNCTION(BlueprintImplementableEvent)
	void PostReplayGotoTime(const bool bWasSuccessful);

	/*
	* Set Play Speed of the Replay
	*/
	UFUNCTION(BlueprintCallable)
	void SetReplaySpeed(float ReplaySpeed = 1.f);


protected:
	FOnGotoTimeDelegate GotoTimeDelegate;

	void OnGotoTimeDelegate(const bool bWasSuccessful);
};

