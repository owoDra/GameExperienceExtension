// Copyright (C) 2024 owoDra

#include "ExperienceDataFactory.h"

#include "ExperienceData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExperienceDataFactory)


UExperienceDataFactory::UExperienceDataFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UExperienceData::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UExperienceDataFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UExperienceData>(InParent, Name, Flags);
}
