// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameWindowCommands.h"

#define LOCTEXT_NAMESPACE "FGameWindowModule"

void FGameWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "GameWindow", "Bring up GameWindow window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ClosePluginWindow, "GameWindow", "Close GameWindow window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
