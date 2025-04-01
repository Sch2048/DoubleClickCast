// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Application/IInputProcessor.h"
#include "Modules/ModuleManager.h"

class DOUBLECLICKCAST_API FDoubleClickCastInputProcessor final
	: public TSharedFromThis<FDoubleClickCastInputProcessor>
	  , public IInputProcessor
{
public:
	static FDoubleClickCastInputProcessor& Get();

	//~ Begin IInputProcessor Interface
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {};
	//~ End IInputProcessor Interface

private:
	TSet<FKey> KeysDown;

	TSharedPtr<FUICommandList> CommandList;
};


class FDoubleClickCastModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr<FDoubleClickCastInputProcessor> InputProcessor;
};
