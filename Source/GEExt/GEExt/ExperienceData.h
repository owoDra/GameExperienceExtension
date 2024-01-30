// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "ExperienceData.generated.h"

class UGameFeatureAction;
class UExperienceActionSet;


/**
 * Data assets that define the game experience in game mode
 */
UCLASS(BlueprintType, Const)
class GEEXT_API UExperienceData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UExperienceData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	inline static const FName NAME_ExperienceDataType{ TEXTVIEW("ExperienceData") };

	/////////////////////////////////////////////////////////////////////////////////////
	// Validate Data
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif 

	/////////////////////////////////////////////////////////////////////////////////////
	// Asset Bundle
public:
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif // WITH_EDITORONLY_DATA

	/////////////////////////////////////////////////////////////////////////////////////
	// Primary Asset
protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Transient, Category = "Debug")
	mutable FString PrimaryAssetId;
#endif

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/**
	 * Return PrimaryAssetId as FString
	 */
	UFUNCTION(BlueprintCallable)
	FString GetIdentifierString() const;


	/////////////////////////////////////////////////////////////////////////////////////
	// Experience Actions
public:
	//
	// List of Game Feature Plugins this experience wants to have active
	//
	UPROPERTY(EditDefaultsOnly, Category = "Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;

	//
	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	//
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	//
	// List of additional action sets to compose into this experience
	//
	UPROPERTY(EditDefaultsOnly, Category = "Actions to Perform")
	TArray<TObjectPtr<UExperienceActionSet>> ActionSets;

};
