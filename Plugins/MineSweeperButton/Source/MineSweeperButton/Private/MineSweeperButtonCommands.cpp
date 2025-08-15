// Copyright Epic Games, Inc. All Rights Reserved.

#include "MineSweeperButtonCommands.h"

#define LOCTEXT_NAMESPACE "FMineSweeperButtonModule"

void FMineSweeperButtonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MineSweeperButton", "Execute MineSweeperButton action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
