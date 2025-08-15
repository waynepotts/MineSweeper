// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameWindow.h"
#include "GameWindowStyle.h"
#include "GameWindowCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Engine/GameViewportClient.h"

static const FName GameWindowTabName("GameWindow");

#define LOCTEXT_NAMESPACE "FGameWindowModule"

void FGameWindowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FGameWindowStyle::Initialize();
	FGameWindowStyle::ReloadTextures();

	FGameWindowCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGameWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FGameWindowModule::PluginButtonClicked),
		FCanExecuteAction());
	PluginCommands->MapAction(
		FGameWindowCommands::Get().ClosePluginWindow,
		FExecuteAction::CreateRaw(this, &FGameWindowModule::ShutdownModule),
		FCanExecuteAction());
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGameWindowModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(GameWindowTabName, FOnSpawnTab::CreateRaw(this, &FGameWindowModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FGameWindowTabTitle", "GameWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	;
	
}

void FGameWindowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
	Board->StopGameTimer();
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FGameWindowStyle::Shutdown();

	FGameWindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(GameWindowTabName);
}

void FGameWindowModule::PreUnloadCallback()
{
}

TSharedRef<SDockTab> FGameWindowModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{

	Board = MakeShared<MineSweeperBoard>();

	TSharedRef <SEditableTextBox> WidthText = SNew(SEditableTextBox)
		.Text(FText::FromString(TEXT("5")))
		.HintText(FText::FromString(TEXT("5")))
		.MinDesiredWidth(250.0f)
		;
	
	WidthText->SetOnKeyCharHandler(
		FOnKeyChar::CreateLambda(
			[this, WidthText, Board = Board](const FGeometry&, const FCharacterEvent& CharEvent) -> FReply
			{
				if (FChar::IsDigit(CharEvent.GetCharacter()))
				{
					return FReply::Unhandled();
				}
				// do nothing if the char is printable but not a number i.e. alphabet or puctuation
				if (FChar::IsPrint(CharEvent.GetCharacter()))
				{
					return FReply::Handled();
				}
				// allow backspace
				return FReply::Handled();
			}
		)
	);
	TSharedRef<class SHorizontalBox> WidthBox = MakeTextEntry(
		FText::FromString(TEXT("Height:"))
		, WidthText
	);
	TSharedRef <SEditableTextBox> HeightText = SNew(SEditableTextBox)
		.Text(FText::FromString(TEXT("5")))
		.HintText(FText::FromString(TEXT("5")))
		.MinDesiredWidth(250.0f);
	HeightText->SetOnKeyCharHandler(
		FOnKeyChar::CreateLambda(
			[this, HeightText, Board = Board](const FGeometry&, const FCharacterEvent& CharEvent) -> FReply
			{
				if (FChar::IsDigit(CharEvent.GetCharacter()))
				{
					return FReply::Unhandled();
				}
				// do nothing if the char is printable but not a number i.e. alphabet or puctuation
				if (FChar::IsPrint(CharEvent.GetCharacter()))
				{
					return FReply::Handled();
				}
				// allow backspace
				return FReply::Unhandled();
			}
		)
	);
	TSharedRef<class SHorizontalBox> HeightBox = MakeTextEntry(
		FText::FromString(TEXT("Height:"))
		, HeightText
	);
	TSharedRef<SHorizontalBox> TopLine = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.HAlign(HAlign_Left)
		[
			WidthBox
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.HAlign(HAlign_Left)
		[
			HeightBox
		];
	TSharedRef <SEditableTextBox> MineText = SNew(SEditableTextBox)
		.Text(FText::FromString(TEXT("5")))
		.HintText(FText::FromString(TEXT("5")))
		.MinDesiredWidth(250.0f)
		;
	MineText->SetOnKeyCharHandler(
		FOnKeyChar::CreateLambda(
			[this, MineText, Board = Board](const FGeometry&, const FCharacterEvent& CharEvent) -> FReply
			{
				if (FChar::IsDigit(CharEvent.GetCharacter()))
				{
					return FReply::Unhandled();
				}
				// do nothing if the char is printable but not a number i.e. alphabet or puctuation
				if (FChar::IsPrint(CharEvent.GetCharacter()))
				{
					return FReply::Handled();
				}
				// allow backspace
				return FReply::Unhandled();
			}
		)
	);
	TSharedRef<SHorizontalBox> MineCount = MakeTextEntry(
		FText::FromString(TEXT("Number Of Mines:"))
		, MineText
	);
	TSharedRef < STextBlock> GameTime = SNew(STextBlock)
		.Text(FText::FromString(TEXT("")))
		.ColorAndOpacity(FLinearColor::White)
		.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12));
	Board->GameTimeText = GameTime;
	TSharedRef<SHorizontalBox> Line2 = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.HAlign(HAlign_Left)
		[
			MineCount
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.HAlign(HAlign_Center)
		[
			GameTime
		];
	TSharedRef <SEditableTextBox> SeedText = SNew(SEditableTextBox)
		.Text(FText::FromString(TEXT("0")))
		.HintText(FText::FromString(TEXT("0")))
		.MinDesiredWidth(250.0f)
		;

	SeedText->SetOnKeyCharHandler(
		FOnKeyChar::CreateLambda(
			[this, WidthText, Board = Board](const FGeometry&, const FCharacterEvent& CharEvent) -> FReply
			{
				if (FChar::IsDigit(CharEvent.GetCharacter()))
				{
					return FReply::Unhandled();
				}
				// do nothing if the char is printable but not a number i.e. alphabet or puctuation
				if (FChar::IsPrint(CharEvent.GetCharacter()))
				{
					return FReply::Handled();
				}
				// allow backspace
				return FReply::Handled();
			}
		)
	);
	SeedText->SetEnabled(bUseSeed);
	TSharedRef<class SHorizontalBox> SeedBox = MakeTextEntry(
		FText::FromString(TEXT("Random Seed:"))
		, SeedText
	);
	TSharedRef<SCheckBox> UseSeed = SNew(SCheckBox)
		.IsChecked(bUseSeed ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Lambda([=, this](ECheckBoxState NewState) -> void
			{
				if (NewState == ECheckBoxState::Checked)
				{
					bUseSeed = true;
					SeedText->SetEnabled(true);
				}
				else
				{
					bUseSeed = false;
					SeedText->SetEnabled(false);
				}
			})
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Use Random Seed")))
			.ColorAndOpacity(FLinearColor::White)
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
		];
	TSharedRef<SHorizontalBox> Line3 = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.HAlign(HAlign_Left)
		[
			UseSeed
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		[
			SeedBox
		]
		;
	TSharedRef<SButton> GenerateBoard = SNew(SButton)
		.Text(FText::FromString(TEXT("Generate New Grid")))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.OnClicked_Lambda([=, this]() -> FReply
			{
				Seed = ToIntValue(SeedText->GetText(), 0);
				Board->RefreshBoard(ToIntValue(WidthText.Get().GetText(), 5)
					, ToIntValue(HeightText.Get().GetText(), 5)
					, ToIntValue(MineText.Get().GetText(), 5)
					, MakeShared<RandomBoardGenerator>(bUseSeed, Seed));
				//, MakeShared<EmptyBoardGenerator>()); // For testing purposes, you can use EmptyBoardGenerator to generate a board without mines
				return FReply::Handled();
			});
	TSharedRef<SVerticalBox> MainPanel = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(10.0f)
		[
			TopLine
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(10.0f)
		[
			Line2
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(10.0f)
		[
			Line3
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(10.0f)
		[
			GenerateBoard
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(10.0f)
		[
			Board->GetVerticalBox()
		]
		;
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SScrollBox)
				+ SScrollBox::Slot()
				.FillSize(1.0f)
				[
					MainPanel
				]

		];
}

TSharedRef<class SHorizontalBox> FGameWindowModule::MakeTextEntry(FText Label, TSharedRef<SEditableTextBox> EditableTextBox)
{
	
	
	TSharedRef<SHorizontalBox> HBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(STextBlock)
				.Text(Label)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			EditableTextBox
		];
	
	return HBox;
}

void FGameWindowModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(GameWindowTabName);
}

int FGameWindowModule::ToIntValue(FText Text, int DefaultValue)
{
	{
		int Value = DefaultValue;
		if (Text.IsNumeric())
		{
			Value = FCString::Atoi(*Text.ToString());
		}
		return Value;
	}
}

void FGameWindowModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FGameWindowCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FGameWindowCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameWindowModule, GameWindow)