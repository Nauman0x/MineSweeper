# Minesweeper Console Game

This is a console-based implementation of the classic **Minesweeper** game written in C++. The game features a grid of hidden mines that the player must uncover while avoiding the mines. 

## Features

- **Interactive gameplay**: Left and right mouse clicks to reveal or flag cells.
- **Grid with Mines**: Randomly generated mines are placed on the grid.
- **Flood Fill Algorithm**: Reveals multiple safe cells when the player clicks on an empty cell.
- **User Data**: Tracks player stats like games played, total wins, and score.
- **Simple UI**: Uses console colors and symbols to represent mines, flags, and uncovered cells.
- **Persistence**: Saves user data to a binary file.

## Game Controls

- **Left Mouse Click**: Reveal a cell.
- **Right Mouse Click**: Flag or unflag a cell.

## Symbols

- **X**: Mine (hidden).
- **0**: Safe cell (uncovered).
- **!**: Flagged cell.
- **■**: Hidden cell (safe but unrevealed).

## How to Play

1. Start the game, and you will be presented with a grid of hidden cells.
2. Left-click to reveal a cell. If it's a mine, you lose. If it's safe, it will uncover that cell and possibly others.
3. Right-click to place or remove a flag on cells you suspect contain mines.
4. The goal is to uncover all safe cells without revealing any mines.

## File Structure

- **minesweeper.cpp**: Main source code file.
- **users.bin**: Binary file storing user information like name, games played, wins, and score.

## Customization

You can customize the grid size, number of mines, and other game parameters by changing the following constants in the code:

```cpp
char GRID_SIZE = 8;  // Grid dimensions
char NUM_MINES = 6;  // Number of mines on the grid



