// Copyright (C) 2023 owoDra

#pragma once

#include "Modules/ModuleManager.h"

/**
 *  Modules for the Editor features of the Game Experience Extension plugin
 */
class FGEExtEditorModule : public IModuleInterface
{
protected:
	typedef FGEExtEditorModule ThisClass;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnBeginPIE(bool bIsSimulating);
	void OnEndPIE(bool bIsSimulating);

};
