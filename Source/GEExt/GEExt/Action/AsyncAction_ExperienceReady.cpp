// Copyright (C) 2024 owoDra

#include "AsyncAction_ExperienceReady.h"

#include "ExperienceDataComponent.h"

#include "InitState/InitStateTags.h"

#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ExperienceReady)


UAsyncAction_ExperienceReady::UAsyncAction_ExperienceReady(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* InWorldContextObject)
{
	if (auto* World{ GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull) })
	{
		auto* Action{ NewObject<UAsyncAction_ExperienceReady>() };
		Action->WorldPtr = World;
		Action->RegisterWithGameInstance(World);
		return Action;
	}

	return nullptr;
}

void UAsyncAction_ExperienceReady::Activate()
{
	if (auto* World{ WorldPtr.Get() })
	{
		if (auto* GameState{ World->GetGameState() })
		{
			Step2_ListenToExperienceLoading(GameState);
		}
		else
		{
			World->GameStateSetEvent.AddUObject(this, &ThisClass::Step1_HandleGameStateSet);
		}
	}
	else
	{
		// No world so we'll never finish naturally

		SetReadyToDestroy();
	}
}

void UAsyncAction_ExperienceReady::Step1_HandleGameStateSet(AGameStateBase* GameState)
{
	if (auto* World{ WorldPtr.Get() })
	{
		World->GameStateSetEvent.RemoveAll(this);
	}

	Step2_ListenToExperienceLoading(GameState);
}

void UAsyncAction_ExperienceReady::Step2_ListenToExperienceLoading(AGameStateBase* GameState)
{
	check(GameState);

	auto* Component{ GameState->FindComponentByClass<UExperienceDataComponent>() };
	check(Component);

	if (Component->HasReachedInitState(TAG_InitState_GameplayReady))
	{
		auto* World{ GameState->GetWorld() };
		check(World);

		// The experience happened to be already loaded, but still delay a frame to
		// make sure people don't write stuff that relies on this always being true
		//@TODO: Consider not delaying for dynamically spawned stuff / any time after the loading screen has dropped?
		//@TODO: Maybe just inject a random 0-1s delay in the experience load itself?

		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::Step4_BroadcastReady));
	}
	else
	{
		Component->OnGameReady_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::Step3_HandleExperienceDataLoaded));
	}
}

void UAsyncAction_ExperienceReady::Step3_HandleExperienceDataLoaded()
{
	Step4_BroadcastReady();
}

void UAsyncAction_ExperienceReady::Step4_BroadcastReady()
{
	OnReady.Broadcast();

	SetReadyToDestroy();
}
