# MinesweeperSolver
Project for a class project of COMPSCI-171 Introduction to Artificial Intelligence at UC Irvine, Spring 2022.

Here is the quick look of the project:

<img src="https://user-images.githubusercontent.com/61955371/195299865-e28b5822-17d3-44be-a7d8-f0ce4ec77bc0.gif" width="50%" height="50%">

## Requirement
Python 3.9 and C++ are required.

## Game Environment
- There are 3 levels of world:
  * `Beginner`: 8x8 with 10 mines
  * `Intermediate`: 16x16 with 40 mines
  * `Expert`: 16x30 with 99 mines


- The starting tile is always safe.
- Mines are randomly placed throughout the board.
- The game end when the AI agent uncovers a mine.

- There are 4 types of action the AI will perform:
  * `Uncover`: Reveal a covered tile.
  * `Flag`: Place a flag on the tile.
  * `Unflag`: Remove a flag on the tile.
  * `Leave`: End the game immediately.


## Create the world
The worlds can be manually created by the txt file in the following format:

```
[rowDimension][space][colDimension]
[startingRow][space][startingColumn]
[2D grid of board]
```

For example, the following txt file will create a 8x8 world with 10 mines with the starting position at (1,8)

```
8 8
1 8
0 0 1 0 0 0 1 0 
0 0 0 1 0 0 0 0 
0 0 0 0 0 0 0 0 
1 0 0 1 0 0 0 0 
0 0 0 0 0 0 0 1 
0 1 0 0 0 1 0 0 
1 0 0 0 0 1 0 0 
0 0 0 0 0 0 0 0 
```

There is a Python script called “WorldGenerator.py” which allow you to generate a set of worlds in the form of txt files
To run the script issue:
```
python3 WorldGenerator.py [numFiles] [filename] [rowDimension]
[colDimension] [numMines]
```
* `numFiles`: The total number of txt files
* `filename`: The basd file name, will start from 1 to numFiles
* `rowDimension/colDimension`: How many row and column in the world
* `numMines`: Total number of mines in the world

Note: Arguments listed below must follow certain restrictions:
* `rowDimension/colDimension`: The minimum value for both rows and columus are 4
* `numMines`: TThe minimum number of mines is 1
* The The number of mines must be less than or equal to (rowDimension)*(colDimension) - 9

