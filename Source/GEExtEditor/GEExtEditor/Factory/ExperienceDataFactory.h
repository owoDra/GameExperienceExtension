// Copyright (C) 2024 owoDra

#pragma once

#include "Factories/Factory.h"

#include "ExperienceDataFactory.generated.h"

class FFeedbackContext;
class UClass;
class UObject;

UCLASS(hidecategories = Object, MinimalAPI)
class UExperienceDataFactory : public UFactory
{
	GENERATED_BODY()
public:
	UExperienceDataFactory(const FObjectInitializer& ObjectInitializer= FObjectInitializer::Get());

public:
	virtual bool ShouldShowInNewMenu() const override { return true; }

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};

