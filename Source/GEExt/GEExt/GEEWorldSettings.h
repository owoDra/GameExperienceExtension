// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/WorldSettings.h"

#include "GEEWorldSettings.generated.h"

class UExperienceData;
struct FPrimaryAssetId;


/**
 * WorldSettings class with ExperienceData items added to WorldSettings that apply to that world.
 */
UCLASS()
class GEEXT_API AGEEWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	AGEEWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//
	// PrimaryAssetId of the default ExperienceData for the world
	// 
	// Tips:
	//	This ExperienceData will be applied unless overridden by session options, etc.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Experience", meta = (AllowedTypes = "ExperienceData"))
	FPrimaryAssetId DefaultExperienceData;

public:
	/**
	 * Returns the PrimaryAssetId of the DefaultExperienceData in world
	 */
	FPrimaryAssetId GetDefaultExperienceData() const { return DefaultExperienceData; }

};
