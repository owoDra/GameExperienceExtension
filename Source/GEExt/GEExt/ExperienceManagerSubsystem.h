// Copyright (C) 2024 owoDra

#pragma once

#include "Subsystems/EngineSubsystem.h"

#include "ExperienceManagerSubsystem.generated.h"


/**
 * Subsystem that tracks GameFeatures modified by Experiences currently being applied in the game.
 * 
 * Tips:
 *	Basically used for execution in PIE
 */
UCLASS(MinimalAPI)
class UExperienceManagerSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	UExperienceManagerSubsystem() {}

private:
	//
	// The map of requests to active count for a given game feature plugin
	// (to allow first in, last out activation management during PIE)
	//
	TMap<FString, int32> GameFeaturePluginRequestCountMap;

public:
#if WITH_EDITOR

	GEEXT_API void OnPlayInEditorBegun();

	static void NotifyOfPluginActivation(const FString PluginURL);
	static bool RequestToDeactivatePlugin(const FString PluginURL);

#else
	static void NotifyOfPluginActivation(const FString PluginURL) {}
	static bool RequestToDeactivatePlugin(const FString PluginURL) { return true; }

#endif

};
