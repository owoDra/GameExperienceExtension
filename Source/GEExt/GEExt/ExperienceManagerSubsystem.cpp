// Copyright (C) 2023 owoDra

#include "ExperienceManagerSubsystem.h"

#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExperienceManagerSubsystem)


#if WITH_EDITOR

void UExperienceManagerSubsystem::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}


void UExperienceManagerSubsystem::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		auto* Subsystem{ GEngine->GetEngineSubsystem<UExperienceManagerSubsystem>() };
		check(Subsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		
		auto& Count{ Subsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL) };
		++Count;
	}
}

bool UExperienceManagerSubsystem::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		auto* Subsystem{ GEngine->GetEngineSubsystem<UExperienceManagerSubsystem>() };
		check(Subsystem);

		// Only let the last requester to get this far deactivate the plugin

		auto& Count{ Subsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL) };
		--Count;

		if (Count == 0)
		{
			Subsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);

			return true;
		}

		return false;
	}

	return true;
}

#endif