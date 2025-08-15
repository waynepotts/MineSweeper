// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/Layout/SBox.h"
#include <vector>

DECLARE_LOG_CATEGORY_EXTERN(MineSweeperLog, Log, All);


/**
* Root interface for generating a Minesweeper board.
* 
* I've kept this as a pure virtual class that uses standard C++ so that it would make it easier to import diffeerent implementations
* that aren't written in Unreal C++. The idea is that this interface can be implemented in any C++ project, and then used in Unreal Engine
*/
class GenerateBoard {
protected:
public:
	virtual std::vector<std::vector<bool>> Generate(int Width, int Height, int NumMines) = 0;
};

/**
* An empty board that we can use for testing purposes.
*/
class EmptyBoardGenerator : public GenerateBoard {
	public:
	std::vector<std::vector<bool>> Generate(int Width, int Height, int NumMines) override {
		return std::vector<std::vector<bool>>(Height, std::vector<bool>(Width, false));
	}
};

/**
* The main generator for the Minesweeper board that generates a random board with mines placed randomly.
* This class uses Unreal's FRandomStream to generate random numbers, and can be seeded for reproducibility.
*/
class RandomBoardGenerator : public GenerateBoard {
public:
	RandomBoardGenerator() {
		bIsSeeded = false;
		Seed = 0;
		RandomStream = FRandomStream(FMath::Rand());
	}
	RandomBoardGenerator(bool bInIsSeeded = false, int InSeed = 0) :  bIsSeeded(bInIsSeeded), Seed(InSeed) {
		if (bIsSeeded) {
			RandomStream = FRandomStream(Seed); // Initialize the random stream with a specific seed
		}
		else {
			RandomStream = FRandomStream(FMath::Rand()); // Initialize the random stream with a random seed
		}
	}
	public:
	std::vector<std::vector<bool>> Generate(int Width, int Height, int NumMines) override {
		std::vector<std::vector<bool>> board(Height, std::vector<bool>(Width, false));
		int placedMines = 0;
		if(bIsSeeded)
		{
			RandomStream.Reset(); // Reset the random stream with the specified seed
		}
		else
		{
			RandomStream = FRandomStream(FMath::Rand()); // Initialize the random stream with a random seed
		}
		while (placedMines < NumMines) {
			
			int row = RandomStream.RandRange(0, Height - 1);
			int col = RandomStream.RandRange(0, Width - 1); 
			if (!board[row][col]) {
				board[row][col] = true;
				placedMines++;
			}
		}
		return board;
	}
	FRandomStream RandomStream;
	bool bIsSeeded = false;
	int Seed = 0;
};

struct MineSweeperTile {
	
	bool bIsMine;
	bool bIsRevealed;
	TSharedRef<SButton> Button;
	MineSweeperTile(int InRow, int InColumn, TSharedRef<SButton> InButton, bool InIsMine = false)
		: bIsMine(InIsMine), bIsRevealed(false), Button(InButton) {}
	MineSweeperTile() : bIsMine(false), bIsRevealed(false), Button(SNew(SButton)) {}
};

/**
* Unreal Engine's TArray doesn't 'like 2D arrays' so we create a wrapper around TArray to simulate a 2D array.
* 
* I don't like that Unreal Engine's TArray doesn't support 2D arrays natively, but wanted to show that I'm aware of this implementation.
* 
* I didn't want to use this struct for two reasons.
* 1. I've always thought it's very bad that Unreal doesn't support 2D arrays natively.
* 2. I decided to use a map instead because its mre flexible and future proof e.g. a 2D array is limited to just 2D. 
* A map could be used for 3D mine sweeper boards, different shaped Tiles (Triagles, or hexagons).
*/
//struct MineSweeperArray {
//public:
//	TArray<MineSweeperTile> Tiles;
//	MineSweeperArray(TArray<MineSweeperTile> InTiles) : Tiles(InTiles) {}
//
//	MineSweeperTile operator[] (int i) {
//		return Tiles[i];
//	}
//	void Add(const MineSweeperTile& Tile) {
//		Tiles.Add(Tile);
//	}
//	void Insert(int Index, const MineSweeperTile& Tile) {
//		Tiles.Insert(Tile, Index);
//	}
//};

/**
 * MAin MineSweeper board class that does all of board management, tile management, and game logic.
 */
class GAMEWINDOW_API MineSweeperBoard 
{

private:
	TMap < FString, TSharedRef<MineSweeperTile>> TileState; // Map to store the state of each tile (e.g., revealed, flagged)
		
	TSharedRef<SHorizontalBox> CreateRow(int Width, int Column, std::vector<std::vector<bool>> Board);
	
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);


	FTSTicker::FDelegateHandle Handle; // Handle for the game timer ticker
	long GameStartTime; // Start time of the game in seconds
	int BoardWidth, BoardHeight, MineNum;
public:
	
	TSharedRef<STextBlock> GameTimeText = SNew(STextBlock);
	~MineSweeperBoard();
	
	void RefreshBoard(int Width, int Height, int NumMines, TSharedPtr<GenerateBoard> Generator);
	TSharedRef<SBox> GetVerticalBox();
	
	void StartGameTimer();
	void StopGameTimer();
	

	void RevealTile(int row, int column);

	int SetTileRevealed(int Row, int Column);

	void PreviewTile(int row, int column, bool bPreview);


	/**
	* Returns the surrounding tiles for a given tile at (Row, Column) within the specified number of rings.
	*/
	TSet<FString> GetSurroundingTiles(int Row, int Column, int Rings);

	/**
	* The drawback to using a map is that we need to keep very tight control of how the keys are generated.
	* 
	* Technically, we could use any string as a key, but we don't want that, we want meaningful keys that represent the tile's position in the board.
	* 
	* Making sure that the keys fit the format of "Row,Column" would be a great to use the new contracts feature in C++26
	*/
	FString MakeKey(int Row, int Column) const
	{
		return FString::Printf(TEXT("%d,%d"), Row, Column);
	}

	/**
	* Same as above, the drawback to using a TMap
	*/
	void SplitKey(const FString& Key, int& Row, int& Column) const
	{
		TArray<FString> Parts;
		Key.ParseIntoArray(Parts, TEXT(","), true);
		if (Parts.Num() == 2)
		{
			Row = FCString::Atoi(*Parts[0]);
			Column = FCString::Atoi(*Parts[1]);
		}
		else
		{
			Row = -1;
			Column = -1;
		}
	}
	void GameOver()
	{
		StopGameTimer();
		for (const auto& TilePair : TileState)
		{
			TilePair.Value->Button->SetBorderBackgroundColor(TilePair.Value->bIsMine ? FLinearColor::Red : FLinearColor::Green);
			TilePair.Value->Button->SetEnabled(false);
		}
	}

	/**
	* The recursive function to get all the surrounding safe tiles. I saw two ways to implement this function
	* 
	* 1. Reveal the tiles as we go, which would be the most efficient way to do it, but its less flexible.
	*	If we want to use the logic in this function for other purposes, like highlighting safe tiles, we would need to refactor the code.
	* 
	* 2. Use a set to store the the tiles we've already visited, and another for the tiles we want to reveal. 
	*/
	TSet<FString> GetSurroundingSafeTiles(int Row, int Column, TSet<FString> &Ignore);

	/**
	* We need to know when the game is over, so we can stop the game timer and disable all the tiles.
	*/
	TSet<FString> GetUnrevealedTiles()
	{
		TSet<FString> UnrevealedTiles;
		for (const auto& TilePair : TileState)
		{
			if (!TilePair.Value->bIsRevealed && !TilePair.Value->bIsMine)
			{
				UnrevealedTiles.Add(TilePair.Key);
			}
		}
		return UnrevealedTiles;
	}
	
};
