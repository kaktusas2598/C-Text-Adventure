# Interactive Text Adventure Game Engine

Fun little experiment started with building Old School text based adventure game in C, based on tutorial series from https://helderman.github.io/htpataic/htpataic01.html, since when I introduced Lua layer to define game logic and objects using which you should be able to implement to create any kind of interactive text adventure game.

# Build instructions

 * Run `make clean`
 * Run `make`
 * Run `textAdventure.exe`

 Or in order to start a new game with a savegame:
  * Run `textAdventure.exe -s savegame.txt` for example, file will be created, run the same command with existing file to load game.

 # Build game map graph

 * Run `make map.gv` 
 * Run `make map.png` 

 # Test instructions
  * Generate transcript from a testscript: `./textAdventure.exe testscript.txt > transcript.txt`
  * Compare baseline with transcript: `diff baseline.txt transcript.txt`
  * Or alternatively run `make success.txt`

# TODO
 * Rewrite more akin to my own style and add comments
 * Remove all references to actual objects in the game, maybe with the exception of the player
 * Possibly savegame could be serialised state instead of command replay however not sure if this is worth it for such a simple single player game?
 * Load every object/location/item from Lua.
 * Replace hard-coded object macros with lookups by `id`.
 * Replace direct function pointers with named Lua or C-registered behaviors.

# Defining a game using Lua fiels

 * To implement game, create Lua file (or multiple ones) as per example below and run using `./textAdventure.exe -o myGame.lua`

# Example of game definition
```lua
return {
    objects = {
        {
            id = "guard",
            kind = "actor",
            description = "a burly guard",
            tags = { "guard", "burly guard" },
            location = "field",
            health = 100,
            condition = "some_named_condition",
            on_open = "some_named_action",
        },
    },
}
```