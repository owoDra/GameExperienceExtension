// Copyright (C) 2023 owoDra

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

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif // WITH_EDITORONLY_DATA

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
