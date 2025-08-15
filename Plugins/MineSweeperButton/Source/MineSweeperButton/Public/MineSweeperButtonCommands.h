// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "MineSweeperButtonStyle.h"

class FMineSweeperButtonCommands : public TCommands<FMineSweeperButtonCommands>
{
public:

	FMineSweeperButtonCommands()
		: TCommands<FMineSweeperButtonCommands>(TEXT("MineSweeperButton"), NSLOCTEXT("Contexts", "MineSweeperButton", "MineSweeperButton Plugin"), NAME_None, FMineSweeperButtonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
