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
 * Add more comments in engine layer
 * Possibly savegame could be serialised state instead of command replay however not sure if this is worth it for such a simple single player game?
 * Simplify tags system?

# Defining a game using Lua fiels

 * To implement game, create Lua file (or multiple ones) as per example below and run using `./textAdventure.exe -g myGame.lua`

# Example of game definition

Simple example, "player" is the only required object which needs to be defined by lua file.

```lua
return {
    objects = {
        field = {
            kind = "location",
            description = "an open field",
            tags = { "field" },
            details = "The field is a nice and quiet place under a clear blue sky.",
            contents = "You see",
            gossip = "A lot of tourists go there.",
            capacity = 9999,
            light = 100,
        },
        silver = {
            kind = "item",
            description = "a silver coin",
            tags = { "silver", "coin", "silver coin" },
            location = "field",
            details = "The coin has an eagle on the obverse.",
            contents = "You see",
            gossip = "Money makes the world go round...",
            weight = 1,
        },
        -- Mandatory to define in world!
        player = {
            kind = "actor",
            description = "yourself",
            tags = { "yourself" },
            location = "field",
            details = "You would need a mirror to look at yourself.",
            contents = "You have",
            gossip = "You're not from around here, are you?",
            health = 100,
            capacity = 20,
            impact = -1,
            -- For permadeath, remove these
            death_destination = "heaven",
            drop_destination = "field",
        },
    },
}
```
