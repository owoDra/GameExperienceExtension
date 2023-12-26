// Copyright (C) 2023 owoDra

#include "ExperienceActionSet.h"

#include "GameFeatureAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExperienceActionSet)


#define LOCTEXT_NAMESPACE "Experience"

UExperienceActionSet::UExperienceActionSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


#if WITH_EDITOR
EDataValidationResult UExperienceActionSet::IsDataValid(TArray<FText>& ValidationErrors)
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(ValidationErrors), EDataValidationResult::Valid) };

	auto EntryIndex{ 0 };
	for (const auto& Action : Actions)
	{
		if (Action)
		{
			Result = CombineDataValidationResults(Result, Action->IsDataValid(ValidationErrors));
		}
		else
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

			ValidationErrors.Add(FText::Format(LOCTEXT("ActionEntryIsNull", "Null entry at index {0} in Actions"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
void UExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (const auto& Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE
