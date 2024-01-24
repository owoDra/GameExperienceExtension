// Copyright (C) 2024 owoDra

#include "AssetTypeActions_ExperienceData.h"

#include "GEExtEditor.h"

#include "ExperienceData.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_ExperienceData::GetSupportedClass() const
{
	return UExperienceData::StaticClass();
}


FText FAssetTypeActions_ExperienceData::GetName() const
{
	return LOCTEXT("ExperienceData", "Experience Data");
}

FColor FAssetTypeActions_ExperienceData::GetTypeColor() const
{
	return FColor(0, 102, 153, 255);
}

uint32 FAssetTypeActions_ExperienceData::GetCategories()
{
	return FGEExtEditorModule::Category;
}

#undef LOCTEXT_NAMESPACE
