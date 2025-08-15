// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "GameWindowStyle.h"

class FGameWindowCommands : public TCommands<FGameWindowCommands>
{
public:

	FGameWindowCommands()
		: TCommands<FGameWindowCommands>(TEXT("GameWindow"), NSLOCTEXT("Contexts", "GameWindow", "GameWindow Plugin"), NAME_None, FGameWindowStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
	TSharedPtr< FUICommandInfo > ClosePluginWindow;
};