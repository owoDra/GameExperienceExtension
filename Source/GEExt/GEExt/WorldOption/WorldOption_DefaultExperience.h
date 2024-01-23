// Copyright (C) 2024 owoDra

#pragma once

#include "WorldSetting/WorldOption.h"

#include "WorldOption_DefaultExperience.generated.h"


/**
 * Override ExperienceData applied to this world
 */
UCLASS(meta = (DisplayName = "Default Experience"))
class GEEXT_API UWorldOption_DefaultExperience : public UWorldOption
{
	GENERATED_BODY()
public:
	UWorldOption_DefaultExperience(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    //
    // PrimaryAssetId of the default ExperienceData for the world
    // 
    // Tips:
    //    This ExperienceData will be applied unless overridden by session options, etc.
    //
    UPROPERTY(EditAnywhere, Category = "Experience", meta = (AllowedTypes = "ExperienceData"))
    FPrimaryAssetId DefaultExperienceData;

public:
    /**
     * Returns the PrimaryAssetId of the DefaultExperienceData in world
     */
    FPrimaryAssetId GetDefaultExperienceData() const { return DefaultExperienceData; }

};
