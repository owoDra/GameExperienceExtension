// Copyright (C) 2024 owoDra

#pragma once

#include "InitState/InitStateComponent.h"

#include "GameFeaturePluginOperationResult.h"

#include "ExperienceDataComponent.generated.h"

class UExperienceData;


/**
 * State of loading and application of ExperienceData
 */
enum class EExperienceLoadState
{
	// ExperienceData has not yet been set and Experience loading has not started
	Unloaded,

	// The Stage of loading assets defined within ExperienceData
	LoadingAssets,

	// The stage of loading the necessary GameFeatures defined in ExperienceData.
	LoadingGameFeatures,

	// The stage to wait for a random delay time for debugging
	LoadingTestingDelay,

	// The stage to execute the GameFeatureAction defined in the ExperienceData.
	ExecutingActions,

	// The stage of complete loading and initialization of all Experience features.
	Loaded,

	// The stage of ending play in the current Experience and invalidating any changes that have been made.
	Deactivating

};


/**
 * Components that manage ExperienceData and initialization state
 */
UCLASS(meta = (BlueprintSpawnableComponent, DisplayName = "Experience Init State Component"))
class GEEXT_API UExperienceDataComponent : public UInitStateComponent
{
	GENERATED_BODY()
public:
	UExperienceDataComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void OnRegister() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual bool CanChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) const override;
	virtual bool CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const override;
	virtual void HandleChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) override;


protected:
	//
	// Data used to spawn Character operated by players or AI
	//
	UPROPERTY(ReplicatedUsing = "OnRep_ExperienceData")
	TObjectPtr<const UExperienceData> ExperienceData{ nullptr };

	//
	// Current ExperienceData loading status
	//
	EExperienceLoadState LoadState{ EExperienceLoadState::Unloaded };

	//
	// URL list of currently loaded GameFeatures
	//
	TArray<FString> GameFeaturePluginURLs;

	//
	// Number of GameFeatures currently loaded
	//
	int32 NumGameFeaturePluginsLoading{ 0 };

	//
	// How many GameFeatureObservers are currently stopped
	//
	int32 NumObservedPausers{ 0 };

	//
	// How many GameFeatureObservers should currently be stopped
	//
	int32 NumExpectedPausers{ 0 };

public:
	/**
	 * Automatically get the best ExperienceData and try to load it
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void TryLoadSuggestedExperienceData();

	/**
	 * Set the current game mode data
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void SetExperienceData(FPrimaryAssetId ExperienceDataId);

protected:
	UFUNCTION()
	virtual void OnRep_ExperienceData();


protected:
	void Step1_LoadExperienceAssets();
	void Step2_LoadExperienceGameFeatures();
	void Step3_TryTestRandomDelay();
	void Step4_ExecuteGameFeatureActions();
	void Step5_CompleteLoadExperience();

	void HandleGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);


protected:
	void DeactivateExperience();

	void HandleActionDeactivationCompleted();
	void HandleAllActionsDeactivated();


public:
	/** 
	 * Gets the game state that owns the component, this will always be valid during gameplay but can return null in the editor 
	 */
	template <class T>
	T* GetGameState() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AGameStateBase>::Value, "'T' template parameter to GetGameState must be derived from AGameStateBase");
		return Cast<T>(GetOwner());
	}

	template <class T>
	T* GetGameStateChecked() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AGameStateBase>::Value, "'T' template parameter to GetGameStateChecked must be derived from AGameStateBase");
		return CastChecked<T>(GetOwner());
	}

	/** 
	 * Gets the game mode that owns this component, this will always return null on the client 
	 */
	AGameModeBase* GetAuthGameMode() const;

	template <class T>
	T* GetGameMode() const
	{
		// Note: Intentionally getting the game mode from the world instead of the game state as it can be null during game state initialization

		static_assert(TPointerIsConvertibleFromTo<T, AGameModeBase>::Value, "'T' template parameter to GetGameMode must be derived from AGameModeBase");
		return Cast<T>(GetAuthGameMode());
	}

};
