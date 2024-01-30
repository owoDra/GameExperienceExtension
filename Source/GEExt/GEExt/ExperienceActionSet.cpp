// Copyright (C) 2024 owoDra

#include "ExperienceActionSet.h"

#include "GameFeatureAction.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExperienceActionSet)

//////////////////////////////////////////////////////////////////////////

UExperienceActionSet::UExperienceActionSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


#if WITH_EDITOR
EDataValidationResult UExperienceActionSet::IsDataValid(FDataValidationContext& Context) const
{
	auto Result{ CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid) };

	auto EntryIndex{ 0 };
	for (const auto& Action : Actions)
	{
		if (Action)
		{
			Result = CombineDataValidationResults(Result, Action->IsDataValid(Context));
		}
		else
		{
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);

			Context.AddError(FText::FromString(FString::Printf(TEXT("Invalid Action defined in Actions[%d] in %s"), EntryIndex, *GetNameSafe(this))));
		}

		++EntryIndex;
	}

#if WITH_EDITORONLY_DATA
	PrimaryAssetId = GetIdentifierString();
#endif

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


FPrimaryAssetId UExperienceActionSet::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(UExperienceActionSet::NAME_ExperienceActionSetType), GetFName());
}

FString UExperienceActionSet::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}
