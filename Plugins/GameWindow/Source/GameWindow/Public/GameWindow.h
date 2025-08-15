// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "MineSweeperBoard.h"

class FToolBarBuilder;
class FMenuBuilder;

class FGameWindowModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void PreUnloadCallback() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	int ToIntValue(FText Text, int DefaultValue);
	
private:

	int Width{ 5 };
	int Height{ 5 };
	int NumMines{ 5 };
	bool bUseSeed{ false };
	int Seed{ 0 };
	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	TSharedRef<class SHorizontalBox> MakeTextEntry(FText Label, TSharedRef<SEditableTextBox> EditableTextBox);

	TSharedPtr<MineSweeperBoard> Board;
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
