// Copyright (C) 2024 owoDra

#include "ExperienceDataComponent.h"

#include "ExperienceDeveloperSettings.h"
#include "ExperienceManagerSubsystem.h"
#include "ExperienceActionSet.h"
#include "ExperienceData.h"
#include "WorldOption/WorldOption_DefaultExperience.h"
#include "GEExtLogs.h"

#include "WorldSetting/GFCWorldSettings.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesSubsystemSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExperienceDataComponent)


UExperienceDataComponent::UExperienceDataComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UExperienceDataComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ExperienceData, Params);
}


void UExperienceDataComponent::OnRegister()
{
	// This component can only be added to classes derived from AGameStateBase

	const auto* GameState{ GetGameState<AGameStateBase>() };
	ensureAlwaysMsgf((GameState != nullptr), TEXT("ExperienceDataComponent on [%s] can only be added to GameState actors."), *GetNameSafe(GetOwner()));

	Super::OnRegister();
}

void UExperienceDataComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeactivateExperience();

	Super::EndPlay(EndPlayReason);
}


bool UExperienceDataComponent::CanChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager) const
{
	if (HasAuthority())
	{
		return (GetGameMode<AGameModeBase>() != nullptr);
	}

	return true;
}

bool UExperienceDataComponent::CanChangeInitStateToDataAvailable(UGameFrameworkComponentManager* Manager) const
{
	if (!ExperienceData)
	{
		return false;
	}

	if (LoadState != EExperienceLoadState::Loaded)
	{
		return false;
	}

	return true;
}

void UExperienceDataComponent::HandleChangeInitStateToSpawned(UGameFrameworkComponentManager* Manager)
{
	TryLoadSuggestedExperienceData();
}


void UExperienceDataComponent::TryLoadSuggestedExperienceData()
{
	// If there is no Authority, do not execute.

	if (!HasAuthority())
	{
		return;
	}

	/**
	 * Getting the ExperienceData to be applied.
	 * 
	 * # Precedence order (highest wins)
	 * 	- Session Options (if present)
	 * 	- URL Options override
	 * 	- Developer Settings override (PIE only)
	 * 	- Command Line override
	 * 	- World Settings
	 *	- Developer Settings defauilt
	 */

	FPrimaryAssetId ExperienceDataId;
	FString ExperienceDataIdSource;

	const auto* World{ GetWorld() };
	const auto* DevSettings{ GetDefault<UExperienceDeveloperSettings>() };

	// See Session Options

	if (auto* GameModeBase{ GetGameMode<AGameModeBase>() })
	{
		auto OptionString{ GameModeBase->OptionsString };

		if (UGameplayStatics::HasOption(OptionString, TEXT("ExperienceData")))
		{
			const auto ExperienceDataIdFromOptions{ UGameplayStatics::ParseOption(OptionString, TEXT("ExperienceData")) };

			ExperienceDataId = FPrimaryAssetId(FPrimaryAssetType(UExperienceData::StaticClass()->GetFName()), FName(*ExperienceDataIdFromOptions));
			ExperienceDataIdSource = TEXT("OptionsString");
		}
	}

	// See Developer Settings override

	if (!ExperienceDataId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceDataId = DevSettings->ExperienceDataOverride;
		ExperienceDataIdSource = TEXT("DeveloperSettings(Override)");
	}

	// See Command Line

	if (!ExperienceDataId.IsValid())
	{
		FString ExperienceDataIdFromCommandLine;

		if (FParse::Value(FCommandLine::Get(), TEXT("ExperienceData="), ExperienceDataIdFromCommandLine))
		{
			ExperienceDataId = FPrimaryAssetId::ParseTypeAndName(ExperienceDataIdFromCommandLine);
			ExperienceDataIdSource = TEXT("CommandLine");
		}
	}

	// See World Settings

	if (!ExperienceDataId.IsValid())
	{
		if (const auto* WorldOption{ AGFCWorldSettings::GetOptionFromWorld<UWorldOption_DefaultExperience>(World) })
		{
			ExperienceDataId = WorldOption->GetDefaultExperienceData();
			ExperienceDataIdSource = TEXT("WorldSettings");
		}
	}

	// See Developer Settings defaults

	if (!ExperienceDataId.IsValid())
	{
		ExperienceDataId = DevSettings->DefaultExperienceData;
		ExperienceDataIdSource = TEXT("DeveloperSettings(Default)");
	}

	// Set ExperienceData

	if (ExperienceDataId.IsValid())
	{
		UE_LOG(LogGameExt_Experience, Log, TEXT("TryLoadSuggestedExperienceData: Suggested ExperienceDataId(%s) by %s"), *ExperienceDataId.ToString(), *ExperienceDataIdSource);
	
		SetExperienceData(ExperienceDataId);
	}
	else
	{
		UE_LOG(LogGameExt_Experience, Error, TEXT("TryLoadSuggestedExperienceData: Could not find valid ExperienceData"));
	}
}

void UExperienceDataComponent::SetExperienceData(FPrimaryAssetId ExperienceDataId)
{
	// If ExperienceDataId is invalid, do not execute.

	if (!ExperienceDataId.IsValid())
	{
		return;
	}

	// If there is no Authority, do not execute.

	if (!HasAuthority())
	{
		return;
	}

	// If ExperienceData is already set, do not execute.

	if (ExperienceData)
	{
		return;
	}

	// Load ExperienceData and begin loading Experience

	auto AssetPath{ UAssetManager::Get().GetPrimaryAssetPath(ExperienceDataId) };
	const auto* NewExperienceData{ Cast<UExperienceData>(AssetPath.TryLoad()) };
	check(NewExperienceData);

	ExperienceData = NewExperienceData;

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ExperienceData, this);

	Step1_LoadExperienceAssets();
}

void UExperienceDataComponent::OnRep_ExperienceData()
{
	Step1_LoadExperienceAssets();
}


void UExperienceDataComponent::Step1_LoadExperienceAssets()
{
	check(ExperienceData);
	check(LoadState == EExperienceLoadState::Unloaded);

	UE_LOG(LogGameExt_Experience, Log, TEXT("[%s] Start load experience from ExperienceData(%s)"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *ExperienceData->GetPrimaryAssetId().ToString());

	// Change current load state to "LoadingAssets"

	LoadState = EExperienceLoadState::LoadingAssets;

	// Cache the primary asset IDs of the assets that need to be loaded

	TSet<FPrimaryAssetId> BundleAssetList;

	BundleAssetList.Add(ExperienceData->GetPrimaryAssetId());

	for (const auto& ActionSet : ExperienceData->ActionSets)
	{
		if (ActionSet)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// Cache the name of the required bundle on the client or server

	TArray<FName> BundlesToLoad;

	const auto OwnerNetMode{ GetOwner()->GetNetMode() };
	const bool bLoadClient{ GIsEditor || (OwnerNetMode != NM_DedicatedServer) };
	const bool bLoadServer{ GIsEditor || (OwnerNetMode != NM_Client) };

	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}

	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	// Start loading assets defined within ExperienceData

	const auto BundleLoadAssetHandle
	{
		UAssetManager::Get().ChangeBundleStateForPrimaryAssets(
			BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority)
	};

	auto OnAssetsLoadedDelegate{ FStreamableDelegate::CreateUObject(this, &ThisClass::Step2_LoadExperienceGameFeatures) };

	if (!BundleLoadAssetHandle.IsValid() || BundleLoadAssetHandle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		BundleLoadAssetHandle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		BundleLoadAssetHandle->BindCancelDelegate(
			FStreamableDelegate::CreateLambda(
				[OnAssetsLoadedDelegate]()
				{
					OnAssetsLoadedDelegate.ExecuteIfBound();
				}
			)
		);
	}
}

void UExperienceDataComponent::Step2_LoadExperienceGameFeatures()
{
	check(ExperienceData);
	check(LoadState == EExperienceLoadState::LoadingAssets);

	UE_LOG(LogGameExt_Experience, Log, TEXT("[%s] Start load GameFeatures by ExperienceData(%s)"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *ExperienceData->GetPrimaryAssetId().ToString());

	// Change current load state to "LoadingGameFeatures"

	LoadState = EExperienceLoadState::LoadingGameFeatures;

	// Collect PluginURLs based on the required GameFeature name defined in ExperienceData.

	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs
	{
		[this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			for (const auto& PluginName : FeaturePluginList)
			{
				FString PluginURL;

				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
				{
					GameFeaturePluginURLs.AddUnique(PluginURL);
				}
				else
				{
					UE_LOG(LogGameExt_Experience, Error, TEXT("Step2_LoadExperienceGameFeatures: Failed to find plugin URL from PluginName(%s) from ExperienceDat(%s)"),
						*PluginName, *Context->GetPrimaryAssetId().ToString());
				}
			}
		}
	};

	CollectGameFeaturePluginURLs(ExperienceData, ExperienceData->GameFeaturesToEnable);
	for (const auto& ActionSet : ExperienceData->ActionSets)
	{
		if (ActionSet)
		{
			CollectGameFeaturePluginURLs(ActionSet, ActionSet->GameFeaturesToEnable);
		}
	}

	// Start loading GameFeatures defined within ExperienceData

	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();

	if (NumGameFeaturePluginsLoading > 0)
	{
		for (const auto& PluginURL : GameFeaturePluginURLs)
		{
			UExperienceManagerSubsystem::NotifyOfPluginActivation(PluginURL);
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::HandleGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		Step3_TryTestRandomDelay();
	}
}

void UExperienceDataComponent::Step3_TryTestRandomDelay()
{
	check(ExperienceData);
	check(LoadState == EExperienceLoadState::LoadingGameFeatures);

	UE_LOG(LogGameExt_Experience, Log, TEXT("[%s] Start try test random delay only with editor while loading experience of ExperienceData(%s)"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *ExperienceData->GetPrimaryAssetId().ToString());

	// Change current load state to "LoadingTestingDelay"

	LoadState = EExperienceLoadState::LoadingTestingDelay;

	// Wait for delay if random delay is required

	const auto* DevSettings{ GetDefault<UExperienceDeveloperSettings>() };
	const auto DelaySecs{ DevSettings->ExperienceRandomLoadDelay };

	if (DelaySecs > 0.0f)
	{
		FTimerHandle DummyHandle;
		GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::Step4_ExecuteGameFeatureActions, DelaySecs, /*bLooping=*/ false);

		return;
	}

	Step4_ExecuteGameFeatureActions();
}

void UExperienceDataComponent::Step4_ExecuteGameFeatureActions()
{
	check(ExperienceData);
	check(LoadState == EExperienceLoadState::LoadingTestingDelay);

	UE_LOG(LogGameExt_Experience, Log, TEXT("[%s] Execute GameFeatureActions in ExperienceData(%s)"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *ExperienceData->GetPrimaryAssetId().ToString());

	// Change current load state to "ExecutingActions"

	LoadState = EExperienceLoadState::ExecutingActions;

	// Execute the actions

	FGameFeatureActivatingContext Context;

	if (const auto* ExistingWorldContext{ GEngine->GetWorldContextFromWorld(GetWorld()) })
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions
	{
		[&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (const auto& Action : ActionList)
			{
				if (Action != nullptr)
				{
					//@TODO: The fact that these don't take a world are potentially problematic in client-server PIE
					// The current behavior matches systems like gameplay tags where loading and registering apply to the entire process,
					// but actually applying the results to actors is restricted to a specific world

					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		}
	};

	ActivateListOfActions(ExperienceData->Actions);
	for (const auto& ActionSet : ExperienceData->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	Step5_CompleteLoadExperience();
}

void UExperienceDataComponent::Step5_CompleteLoadExperience()
{
	check(ExperienceData);
	check(LoadState == EExperienceLoadState::ExecutingActions);

	UE_LOG(LogGameExt_Experience, Log, TEXT("[%s] Completed loading of Experience by ExperienceData(%s)"), HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *ExperienceData->GetPrimaryAssetId().ToString());

	// Change current load state to "Loaded"

	LoadState = EExperienceLoadState::Loaded;

	CheckDefaultInitialization();
}

void UExperienceDataComponent::HandleGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		Step3_TryTestRandomDelay();
	}
}


void UExperienceDataComponent::DeactivateExperience()
{
	// Deactivate any features this experience loaded

	auto& GameFeatureSubsystem{ UGameFeaturesSubsystem::Get() };

	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		if (UExperienceManagerSubsystem::RequestToDeactivatePlugin(PluginURL))
		{
			GameFeatureSubsystem.DeactivateGameFeaturePlugin(PluginURL);
		}
	}

	// Deactivate any actions this experience loaded

	if (ExperienceData && (LoadState == EExperienceLoadState::Loaded))
	{
		LoadState = EExperienceLoadState::Deactivating;

		// Make sure we won't complete the transition prematurely if someone registers as a pauser but fires immediately

		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// Deactivate and unload the actions

		auto Context{ FGameFeatureDeactivatingContext(TEXT(""), [this](FStringView) { this->HandleActionDeactivationCompleted(); }) };

		if (const auto* ExistingWorldContext{ GEngine->GetWorldContextFromWorld(GetWorld()) })
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		/*auto DeactivateListOfActions
		{
			[&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (const auto& Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}
			}
		};*/

		DeactivateListOfActions(Context, ExperienceData->Actions);
		for (const auto& ActionSet : ExperienceData->ActionSets)
		{
			if (ActionSet)
			{
				DeactivateListOfActions(Context, ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogGameExt_Experience, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in ExperienceData"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			HandleAllActionsDeactivated();
		}
	}
}

void UExperienceDataComponent::DeactivateListOfActions(FGameFeatureDeactivatingContext& Context, const TArray<UGameFeatureAction*>& ActionList)
{
	/*for (const auto& Action : ActionList)
	{
		if (Action)
		{
			Action->OnGameFeatureDeactivating(Context);
			Action->OnGameFeatureUnregistering();
		}
	}*/
}

void UExperienceDataComponent::HandleActionDeactivationCompleted()
{
	check(IsInGameThread());

	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		HandleAllActionsDeactivated();
	}
}

void UExperienceDataComponent::HandleAllActionsDeactivated()
{
	LoadState = EExperienceLoadState::Unloaded;
	ExperienceData = nullptr;
}


AGameModeBase* UExperienceDataComponent::GetAuthGameMode() const
{
	auto* GS{ GetGameStateChecked<AGameStateBase>() };

	return GS->AuthorityGameMode;
}
