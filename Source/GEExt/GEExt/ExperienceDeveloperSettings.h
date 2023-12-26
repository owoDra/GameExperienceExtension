// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DeveloperSettings.h"

#include "ExperienceDeveloperSettings.generated.h"


/**
 * Developer settings for game experience
 */
UCLASS(Config = "Game")
class UExperienceDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UExperienceDeveloperSettings() {}

public:
	virtual FName GetCategoryName() const override;

public:
	//
	// Default ExperienceData for the overall game.
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Experience", meta = (AllowedTypes = "ExperienceData"))
	FPrimaryAssetId DefaultExperienceData;

	//
	// ExperienceData that forcibly overwrites the set ExperienceData
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Experience", meta = (AllowedTypes = "ExperienceData"))
	FPrimaryAssetId ExperienceDataOverride;

	//
	// ExperienceData that forcibly overwrites the set ExperienceData
	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, Category = "Experience|Debug", meta = (ClampMin = 0.0))
	float ExperienceRandomLoadDelay{ 0.0f };

};
