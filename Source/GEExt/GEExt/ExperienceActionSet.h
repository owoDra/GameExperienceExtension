// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "ExperienceActionSet.generated.h"

class UGameFeatureAction;


/**
 * Data assets that summarize actions that are common across multiple ExperienceData
 */
UCLASS(BlueprintType, Const)
class UExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UExperienceActionSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	inline static const FName NAME_ExperienceActionSetType{ TEXTVIEW("ExperienceActionSet") };

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
#endif 

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
	UPROPERTY(EditAnywhere, Category = "Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;

	//
	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	//
	UPROPERTY(EditAnywhere, Instanced, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

};
