This is a version of Mine Sweeper that is played to in the Unreal Editor. I've coded it for flexibility over performance because I feel its more import to be able to add new things that worry how many clock pulses we're using to traverse a hash map compared to an array.

Good things about this implmentation
  - I think it's a solid foundation where new features could be added quickly
  - Because I'm using a hash map to manage the board we're not limited to the 2D grid of squares that a we would have been had I used arrays,

Bad things
  - Maps are slower than arrays
  - I don't like the way the timer needs to keep rechecking that the window is still open every second.

AI/LLM Usage in this project

I used Github Copilot through Visual Studio for code suggestions like formatting lambdas etc. It did provide some useful code completions, but it also made up plenty of rubbish where it tried to pass of fuction calls and variables that don't exist as valid code completions. It was around 50% useful and 50% useless.

Other than that I asked ChatGPT for ideas of extra functions to add to Mine Sweeper, chat log below.


what extra functions could be added to improve a minesweeper game
ChatGPT zei:

Here are some extra functions and features you could add to a Minesweeper game to make it more engaging, user-friendly, and replayable:

Gameplay Improvements

Hint System

A limited number of hints that reveal a safe square or a possible mine location.

Could reduce the player’s score when used.

Undo / Step Back

Allows the player to revert their last move (especially useful for accidental clicks).

Safe Start

Ensure the first click is never a mine and automatically reveals a large area.

Custom Difficulty

Let players set grid size, number of mines, and even mine density.

Multiple Mine Types

Mines with different blast ranges (e.g., normal mine affects 1 tile radius, “super” mine affects 2).

Timed Bombs

Certain mines detonate after a time limit unless flagged in time.

Quality of Life Features

Auto-Flag & Auto-Reveal

If all adjacent mines are flagged, automatically reveal the rest.

If all safe squares are revealed, automatically flag remaining mines.

Keyboard Shortcuts / Controls

Use keys for flagging, revealing, hints, or zooming.

Resizable & Zoomable Board

Especially for large boards—scroll and zoom for easier navigation.

Highlight Suspicious Zones

Optional overlay showing potential mine areas based on current flags.

Touchscreen Optimization

Tap to reveal, long-press to flag (good for mobile versions).

Visual & Audio Enhancements

Custom Skins / Themes

Change tile appearance (e.g., retro, modern, neon, pixel art).

Dynamic backgrounds that change with progress.

Animations

Smooth reveal animations, exploding mines, flag waving.

Sound & Music

Different sound effects for reveal, flag, and win/lose.

Tension-building background music.

Scoring & Competition

Leaderboard

Local and online leaderboards for time, accuracy, and minimal clicks.

Achievements / Challenges

Win without using flags, win under a time limit, etc.

Replay Recording

Record games and allow players to watch or share their runs.

Creative Variations

3D Minesweeper

Add depth so mines exist in multiple layers (cube or sphere).

Hexagonal or Triangular Grids

Change grid geometry for fresh gameplay.

Campaign Mode

Levels that increase in difficulty, with objectives beyond “clear all mines.”

Minesweeper + Puzzle Hybrid

Add logic puzzles, obstacles, or collectible items inside the grid.

If you want, I could put together a ranked list of which features would make the biggest improvement to gameplay without overwhelming new players. That way you’d know which ones are worth adding first.



