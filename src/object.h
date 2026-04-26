#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <stddef.h> // For size_t

typedef struct Object Object;

typedef bool (*ObjectCondition)(Object* object);
typedef void (*ObjectAction)(Object* object);

// Used to store Ids to other Object* each Object might reference too and used later to resolve those references
// into Object pointers such as location, destination or lua ref to method engine needs to call.
typedef struct LuaObjectMetadata {
    char* locationId;
    char* destinationId;
    char* prospectId;

    char* togglesToId;
    char* mirrorsToId;
    char* locksToId;
    char* keyId;

    // TODO: probably only need int refs to lua here?
    char* condition;
    int conditionRef;
    char* onOpen;
    int onOpenRef;
    char* onClose;
    int onCloseRef;
    char* onLock;
    int onLockRef;
    char* onUnlock;
    int onUnlockRef;

    char* deathDestinationId; 
    char* dropDestinationId;  
} LuaObjectMetadata;


// Main structure representing each Object in a text adventure game
struct Object {
    char* id;
    ObjectCondition condition;
    char* description;
    const char** tags;
    size_t tagCount; // Currently not used anywhere
    char *kind; // Location, Actor, Passage, Item or Container

    Object* location;
    Object* destination;
    Object* prospect; // Optional field for objects that have a different prospect than destination, e.g. a door that leads to a different location than it prospects

    Object *deathDestination; // Optional field for where the player should be moved to when they die
    Object* dropDestination;  // Optional field for where the player's objects should be moved to when they die

    Object* togglesTo; // Optional alternate state, e.g. lamp on/off or box open/closed
    Object* mirrorsTo; // Optional linked state on the other side, e.g. paired doors
    Object* locksTo;   // Optional locked/unlocked state partner
    Object* key;       // Optional key object required for lock/unlock

    char* details;
    char* contents;
    char* textGo; // Optional text to show when player tries to go to this object, e.g. "You can't go there, it's a wall."
    char* gossip;

    int weight;
    int capacity; // How much does an object can contain (eg. actor pockets or container)
    int health;
    int light;
    int impact; // Optional field for attackable objects that determines how much damage they deal when attacking the player, or how much damage they take when attacked by the player
    int trust; // Optional field for talkable objects that determines how much they trust the player, which can be used to gate certain actions or dialogue options

    // Actions defined by Lua game file, or default actions if not defined by Lua game file
    ObjectAction open;
    ObjectAction close;
    ObjectAction lock;
    ObjectAction unlock;

    LuaObjectMetadata luaMetadata;
};

// TODO: either keep using this in the engine or just use World struct?
extern Object* objs;
extern Object* endOfObjs;
extern Object* player;

#define validObject(obj)     ((obj) != NULL && (obj)->condition(obj))

Object* objectById(const char* id);


bool resolveReferences(Object* object);
void initializeRuntimeFields(Object* object);

#endif
