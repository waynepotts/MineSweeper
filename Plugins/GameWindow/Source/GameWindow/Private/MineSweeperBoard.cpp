// Fill out your copyright notice in the Description page of Project Settings.


#include "MineSweeperBoard.h"

#include "Containers/Ticker.h"

DEFINE_LOG_CATEGORY(MineSweeperLog);

MineSweeperBoard::~MineSweeperBoard()
{
	StopGameTimer();
}

void MineSweeperBoard::RefreshBoard(int Width, int Height, int NumMines, TSharedPtr<GenerateBoard> Generator)
{
	VerticalBox->ClearChildren();

	TileState.Empty(); // Clear the TileState for each new row
	BoardWidth = Width;
	BoardHeight = Height;
	// Create a new board using the generator
	std::vector<std::vector<bool>> Board = Generator->Generate(Width, Height, NumMines);
	
	// For testing purposes, you can set mines manually in the board,
	//Board[2][2] = true; // Example: Set a mine at (2, 2) for testing purposes
	//Board[2][0] = true; // Example: Set a mine at (0, 0) for testing purposes
	//Board[2][4] = true; // Example: Set a mine at (4, 4) for testing purposes


	for (int i = 0; i < Height; i++)
	{
		// Create a new row for each height
		TSharedRef<SHorizontalBox> Row = CreateRow(Width, i, Board);
		VerticalBox->AddSlot()
			.AutoHeight()
			[
				Row
			];
	}
	StopGameTimer();
	StartGameTimer();
}
TSharedRef<SBox> MineSweeperBoard::GetVerticalBox()
{
	TSharedRef<SBox> BoardBox = SNew(SBox)
		[
			VerticalBox
		];

	return BoardBox;

}

void MineSweeperBoard::StartGameTimer()
{
	GameStartTime = FPlatformTime::Seconds();
	FTickerDelegate Delegate = FTickerDelegate::CreateLambda([&](float DeltaTime)
		{
			if (!FGlobalTabmanager::Get()->FindExistingLiveTab(FName("GameWindow")).IsValid())
			{
				// The delegate dosn't shutdown when we close the window, so we need to check if the game window is still open
				GameOver();
				return false;
			}
			long TimeElapsed = FPlatformTime::Seconds() - GameStartTime;
			FTimespan TimeSpan = FTimespan::FromSeconds(TimeElapsed);
			//UE_LOG(MineSweeperLog, Log, TEXT("Time: %ld"), TimeElapsed);
			GameTimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %02d:%02d:%02d"), TimeSpan.GetHours(), TimeSpan.GetMinutes(), TimeSpan.GetSeconds())));

			return true;
		});
	Handle = FTSTicker::GetCoreTicker().AddTicker(Delegate, 1.0f); // Delay of 1 second for each tick
	
}

void MineSweeperBoard::StopGameTimer()
{
		if (Handle.IsValid())
		{
			UE_LOG(MineSweeperLog, Log, TEXT("Stopping game timer"));
			FTSTicker::GetCoreTicker().RemoveTicker(Handle);
		}
}

TSharedRef<SHorizontalBox> MineSweeperBoard::CreateRow(int Width, int Row, std::vector<std::vector<bool>> Board)
{
	const int RowIndex = Row; // Capture the current column index
	TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);
	for(int i = 0; i < Width; i++)
	{
		const int ColumnIndex = i; // Capture the current column index
		// Create a button or widget for each cell in the row
		TSharedRef<SButton> CellButton = SNew(SButton)
			.Text(FText::FromString("+"))
			.ForegroundColor(FLinearColor::Gray)
			// Decided to use on pressed and released because it's more flexible that just on clicked and we may want to do different things on pressed and released
			.OnPressed_Lambda([=, this, RowIndex = RowIndex, ColumnIndex = ColumnIndex]() {
			
			for (const auto& Key : this->GetSurroundingTiles(RowIndex, ColumnIndex, 1))
			{
				TileState[Key]->Button->SetBorderBackgroundColor(FLinearColor::Blue);
			}
				
				})
			.OnReleased_Lambda([=, this, RowIndex = RowIndex, ColumnIndex = ColumnIndex]() {
			for (const auto& Key : this->GetSurroundingTiles(RowIndex, ColumnIndex, 1))
			{
				TileState[Key]->Button->SetBorderBackgroundColor(FLinearColor::Gray);
			}
			this->RevealTile(RowIndex, ColumnIndex);
				})
			;

		// The alternative to using OnPressed and OnReleased is to use OnClicked, but it doesn't allow for previewing the tile before clicking
		//CellButton->SetOnClicked(
		//	FOnClicked::CreateLambda(
		//		[=]() -> FReply
		//		{
		//			return FReply::Handled();
		//		}
		//	)
		//);

		TileState.Add(MakeKey(RowIndex, ColumnIndex), MakeShared<MineSweeperTile>(MineSweeperTile(RowIndex, ColumnIndex, CellButton, Board[RowIndex][ColumnIndex])));

		HorizontalBox->AddSlot()
			.AutoWidth()
			[
				CellButton
			];
	}
	return HorizontalBox;
}

void MineSweeperBoard::RevealTile(int Row, int Column)
{
	TSharedRef < MineSweeperTile> Tile = *TileState.Find(MakeKey(Row, Column));
	if(!Tile->bIsMine)
	{
		int MineCount = SetTileRevealed(Row, Column);
		
		if(MineCount == 0)
		{
			TSet<FString> Safe;
			TSet<FString> Checked;
			Safe = GetSurroundingSafeTiles(Row, Column, Checked);
			for (const auto& Key : Safe)
			{
				
				int RowIndex, ColumnIndex;
				SplitKey(Key, RowIndex, ColumnIndex);
				SetTileRevealed(RowIndex, ColumnIndex);
			}
		}
		if(GetUnrevealedTiles().Num() == 0)
		{
			GameOver();
			UE_LOG(MineSweeperLog, Log, TEXT("You Win!"));
			FText WinText = FText::FromString(TEXT("You Win!"));
			FMessageDialog::Open(EAppMsgType::Ok, WinText);
			
		}
	}
	else
	{
		GameOver();
		UE_LOG(MineSweeperLog, Log, TEXT("You Lost!"));
		FText WinText = FText::FromString(TEXT("You Lost!"));
		FMessageDialog::Open(EAppMsgType::Ok, WinText);
		
	}
}

int MineSweeperBoard::SetTileRevealed(int Row, int Column)
{
	TSharedRef < MineSweeperTile> Tile = *TileState.Find(MakeKey(Row, Column));
	int MineCount = GetSurroundingTiles(Row, Column, 1).Array().FilterByPredicate([this](const FString& Key) {
		return TileState.Find(Key)->Get().bIsMine;
		}).Num();
	Tile->bIsRevealed = true;
	Tile->Button->SetBorderBackgroundColor(Tile->bIsMine ? FLinearColor::Red :FLinearColor::Green);
	Tile->Button->SetContent(
		SNew(STextBlock)
		.Text(FText::FromString(FString::FromInt(MineCount)))
		.ColorAndOpacity(FLinearColor::White)
	);
	if (MineCount == 0)
	{
		Tile->Button->SetEnabled(false);
	}
	return MineCount;
}

void MineSweeperBoard::PreviewTile(int row, int column, bool bPreview)
{
	TSharedRef < MineSweeperTile> Tile = *TileState.Find(MakeKey(row, column));
	if (!Tile->bIsRevealed)
	{
		FLinearColor Color = bPreview ? FLinearColor::Blue : FLinearColor::Gray;
		Tile->Button->SetBorderBackgroundColor(Color);
	}
}

TSet<FString> MineSweeperBoard::GetSurroundingTiles(int Row, int Column, int Rings)
{
	/*
	* As we're essentially USING a 2D array, we need to ensure that we don't go out of bounds, So we're clamping with min and max functions.
	*/
	int MinRow = FMath::Max(0, Row - Rings);
	int MaxRow = FMath::Min(Row + Rings, BoardHeight - 1);
	int MinColumn = FMath::Max(0, Column - Rings);
	int MaxColumn = FMath::Min(Column + Rings, BoardWidth - 1);
	TSet<FString> AllTiles;
	for (int i = MinRow; i <= MaxRow; i++) {
		
		for (int j = MinColumn; j <= MaxColumn; j++) {
			// Iterate through each child in the row
			// Check if the child is a button or widget you want to add
			
			if(i != Row || j != Column) // Exclude the center tile
			{
				FString Key = MakeKey(i, j);
				TSharedRef<MineSweeperTile> Tile = *TileState.Find(Key);
				if(!Tile->bIsRevealed)
				{
					AllTiles.Add(Key);
				}
			}
		}
	}
	// I prefer using sets for this because they are unique and we don't have to worry about duplicates.
		
	return AllTiles;

}

TSet<FString> MineSweeperBoard::GetSurroundingSafeTiles(int Row, int Column, TSet<FString> &Checked)
{
	TSet<FString> Found;
	FString MyKey = MakeKey(Row, Column);
	if (Checked.Contains(MyKey) || TileState.Find(MyKey)->Get().bIsMine)
	{
		return TSet<FString>(); // Return early if already checked or revealed
	}
	Found.Add(MyKey);
	Checked.Add(MyKey);
	int MineCount = GetSurroundingTiles(Row, Column, 1).Array().FilterByPredicate([this](const FString& Key) {
		return TileState.Find(Key)->Get().bIsMine;
		}).Num();
	if (MineCount == 0)
	{

		//RevealTile(Row, Column);
		TArray<FString> Array = GetSurroundingTiles(Row, Column, 1).Array().FilterByPredicate([this, Checked](const FString& Key) {
				return !TileState.Find(Key)->Get().bIsMine && !Checked.Contains(Key);
				});
			
		for (const FString& Key : Array)
		{
			if (!Checked.Contains(Key))
			{
				int NextRow, NextColumn;
				SplitKey(Key, NextRow, NextColumn);
				Found = Found.Union(GetSurroundingSafeTiles(NextRow, NextColumn, Checked));
			}
		}
	} 
	return Found;
}

