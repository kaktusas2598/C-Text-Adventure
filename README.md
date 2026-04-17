# C Text Adventure Game

Fun little experiment building Old School text based adventure game in C, based on tutorial series from https://helderman.github.io/htpataic/htpataic01.html

# Build instructions

 * Run `make clean`
 * Run `make`
 * Run `textAdventure.exe`

 Or in order to start a new game with a savegame:
  * Run `textAdventure.exe savegame.txt` for example, file will be created, run the same command with existing file to load game.

 # Build game map graph

 * Run `make map.gv` 
 * Run `make map.png` 

# TODO
 * Rewrite more akin to my own style and add comments
 * Replace awk code generation with something better, maybe a data design based approach or perhaps Lua
 * Remove all references to actual objects in the game, maybe with the exception of the player
 * Possibly savegame could be serialised state instead of command replay however not sure if this is worth it for such a simple single player game?