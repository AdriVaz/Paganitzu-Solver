# Paganitzu Solver

This program is capable of solving levels of the classical game Paganitzu, where the player has to pick up all the keys in the map without getting caught by any of the snakes. The player is able to move rocks to put them between himself and the snakes, so the snakes don't shoot and kill the player.

The solving is done using the A Star algorithm

## Compilation

The program can be compiled just by running `make`

By running `make clean` all the binary files, `.output` and `.statistics` files are deleted, basically returning the repo to its initial state

## Usage

The program can be launched with the following arguments:
```
./AstarPaganitzu <map file> <heuristic index>
```
Where:
- `<map file>` is a file with an encoded labyrinth. There are some examples inside the `maps` folder
- `<heuristic id>` is either 0 or 1. 0 Uses the Manhattan distance heuristic, and 1 uses the Euclidean distance.

## Output

The program prints the output on the screen, but also writes them into two files, named after the map name, but ended in `.output` and `.statistics` respectively.

