// Copyright (C) 2024 owoDra

#include "ExperienceData.h"

#include "GameFeatureAction.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExperienceData)

//////////////////////////////////////////////////////////////////////////

UExperienceData::UExperienceData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


#if WITH_EDITOR
EDataValidationResult UExperienceData::IsDataValid(FDataValidationContext& Context) const
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
#endif

#if WITH_EDITORONLY_DATA
void UExperienceData::UpdateAssetBundleData()
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


FPrimaryAssetId UExperienceData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(UExperienceData::NAME_ExperienceDataType), GetFName());
}

FString UExperienceData::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}
