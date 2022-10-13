# MinesweeperSolver
## Table of contents
* [Introduction](#introduction)
* [Requirement](#requirement)
* [Game Environment](#game-environment)
* [Create the World](#create-the-world)
* [Shell Manual](#shell-manual)
* [Performance](#performance)
* [Conclusion](#conclusion)


## Introduction
Project for a class project of COMPSCI-171 Introduction to Artificial Intelligence at UC Irvine, Spring 2022.

The AI agent will solve the given world automatically, and provide the result & related data to the user.

Here is the quick look of the project:

<img src="https://user-images.githubusercontent.com/61955371/195299865-e28b5822-17d3-44be-a7d8-f0ce4ec77bc0.gif" width="50%" height="50%">

## Requirement
Python 3.9 and C++ are required.

## Game Environment
- There are 4 levels of world by default in `WorldGenerator/Problems` (1000 worlds per level):
  * `Easy`: 5x5 with 1 mine
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


## Create the World
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


## Shell Manual
Issue the command after `make`:
```
[Name] [Options] [InputFile] [OutputFile]
```
- `Name`: The name is `./Minesweeper` while using cpp shell
- `Options`: 
  * `-d`: Debug mode, which will also display the game board after each move.
  * `-v`: Verbose mode, which displays name of world files as they are loaded.
  * `-f`: Depending on the InputFile format supplied, this operand will trigger program:
    * Treats the InputFile as a folder containing many worlds. The program will then construct a world for every valid world file found. The program to display total score instead of a single score. The InputFile operand must be specified with this option
    * Threats the inputFile as a file. The program will then construct a world for a single valid world file found. The program to display a single score.
- `Operands`:
  * `InputFile`: A path to a valid Minesweeper World file, or folder with `–f`. This operand is optional unless used with `–f` or OutputFile.
  * `OutputFile`: A path to a file where the results will be written. This is optional.


## Performance
The following data are plotted from 1000 tests per level

- Level vs. Accuracy

![download](https://user-images.githubusercontent.com/61955371/195501983-9ed7ae29-1903-49d5-ae25-997cf7cdd716.png)

- Time vs. Level

![download-1](https://user-images.githubusercontent.com/61955371/195502144-0986f6e6-736e-42d4-b110-4f19be1c39b1.png)


## Conclusion

- The accuracy dropped rapidly in the `expert level`, the possible reason might be that as the world's size and number of mines increase, the chance we need to guess also increase. Our AI will calculate the probability of being a mines for every neighbor tile when there is no tile which is 100% safe. Therefore, we can improve the guess algorithm in the agent to a better guess when we have to.  

- The running time didn't increase dramticly when the world is relevant small (5x5, 8x8, and 16x16), but in `expert level` the running time increased rapidly. The running time increase exponentially as the size of the world increase. The AI adopted recursively search algorithm and trim the redundant possible worlds in the tree. However, this algorithm seems not to be efficiently when the world's size increase.
