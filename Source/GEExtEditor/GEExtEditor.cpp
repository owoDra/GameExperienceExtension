// Copyright (C) 2024 owoDra

#include "GEExtEditor.h"

#include "ExperienceManagerSubsystem.h"

#include "Editor.h"

IMPLEMENT_MODULE(FGEExtEditorModule, GEExtEditor)


void FGEExtEditorModule::StartupModule()
{
	if (!IsRunningGame())
	{
		FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
		FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
	}
}

void FGEExtEditorModule::ShutdownModule()
{
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
