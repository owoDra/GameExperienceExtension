// Copyright (C) 2024 owoDra

#include "GEExtEditor.h"

#include "ExperienceManagerSubsystem.h"

#include "AssetTypeActions/AssetTypeActions_ExperienceData.h"

#include "Editor.h"

IMPLEMENT_MODULE(FGEExtEditorModule, GEExtEditor)


void FGEExtEditorModule::StartupModule()
{
	RegisterAssetTypeActions();

	if (!IsRunningGame())
	{
		FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
		FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
	}
}

void FGEExtEditorModule::ShutdownModule()
{
	UnregisterAssetTypeActions();
}


void FGEExtEditorModule::OnBeginPIE(bool bIsSimulating)
{
	auto* ExperienceManager{ GEngine->GetEngineSubsystem<UExperienceManagerSubsystem>() };
	check(ExperienceManager);

	ExperienceManager->OnPlayInEditorBegun();
}

void FGEExtEditorModule::OnEndPIE(bool bIsSimulating)
{
}


void FGEExtEditorModule::RegisterAssetTypeActionCategory()
{
	static const FName CategoryKey{ TEXT("GameGameMode") };
	static const FText CategoryDisplayName{ NSLOCTEXT("GameXXX", "GameGameModeCategory", "GameMode") };

	Category = IAssetTools::Get().RegisterAdvancedAssetCategory(CategoryKey, CategoryDisplayName);
}

void FGEExtEditorModule::RegisterAssetTypeActions()
{
	RegisterAssetTypeActionCategory();

	RegisterAsset<FAssetTypeActions_ExperienceData>(RegisteredAssetTypeActions);
}

void FGEExtEditorModule::UnregisterAssetTypeActions()
{
	UnregisterAssets(RegisteredAssetTypeActions);
}

void FGEExtEditorModule::UnregisterAssets(TArray<TSharedPtr<FAssetTypeActions_Base>>& RegisteredAssets)
{
	const auto* AssetToolsPtr{ FModuleManager::GetModulePtr<FAssetToolsModule>(NAME_AssetToolsModule) };
	if (!AssetToolsPtr)
	{
		return;
	}

	auto& AssetTools{ AssetToolsPtr->Get() };
	for (auto& AssetTypeActionIt : RegisteredAssets)
	{
		if (AssetTypeActionIt.IsValid())
		{
			AssetTools.UnregisterAssetTypeActions(AssetTypeActionIt.ToSharedRef());
		}
	}
}
