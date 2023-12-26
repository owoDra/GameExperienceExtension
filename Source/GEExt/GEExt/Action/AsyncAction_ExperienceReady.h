// Copyright (C) 2023 owoDra

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncAction_ExperienceReady.generated.h"

class UExperienceData;
class UGameStateBase;


/**
 * Delegate to notify that ExperienceData has been initialized
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExperienceDataReadyAsyncDelegate);


/**
 * Asynchronously waits for the game state to be ready and valid and then calls the OnReady event.  Will call OnReady
 * immediately if the game state is valid already.
 */
UCLASS()
class UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	TWeakObjectPtr<UWorld> WorldPtr;

public:
	//
	// Called when the experience has been determined and is ready/loaded
	//
	UPROPERTY(BlueprintAssignable)
	FExperienceDataReadyAsyncDelegate OnReady;

public:
	/**
	 * Waits for the experience to be determined and loaded
	 */
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", BlueprintInternalUseOnly="true"))
	static UAsyncAction_ExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

	virtual void Activate() override;

private:
	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToExperienceLoading(AGameStateBase* GameState);
	void Step3_HandleExperienceDataLoaded();
	void Step4_BroadcastReady();

};
