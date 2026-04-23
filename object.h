#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>

typedef struct LuaWorldObject LuaWorldObject;
typedef struct Object Object;

typedef bool (*ObjectCondition)(Object* object);
typedef void (*ObjectAction)(Object* object);

struct Object {
    const char* id;
    ObjectCondition condition;
    const char* description;
    const char** tags;

    Object* location;
    Object* destination;
    Object* prospect; // Optional field for objects that have a different prospect than destination, e.g. a door that leads to a different location than it prospects

    Object* togglesTo; // Optional alternate state, e.g. lamp on/off or box open/closed
    Object* mirrorsTo; // Optional linked state on the other side, e.g. paired doors
    Object* locksTo;   // Optional locked/unlocked state partner
    Object* key;       // Optional key object required for lock/unlock

    const char* details;
    const char* contents;
    const char* textGo; // Optional text to show when player tries to go to this object, e.g. "You can't go there, it's a wall."
    const char* gossip;

    int weight;
    int capacity;
    int health;
    int light;
    int impact; // Optional field for attackable objects that determines how much damage they deal when attacking the player, or how much damage they take when attacked by the player
    int trust; // Optional field for talkable objects that determines how much they trust the player, which can be used to gate certain actions or dialogue options

    // Actions defined by Lua game file, or default actions if not defined by Lua game file
    ObjectAction open;
    ObjectAction close;
    ObjectAction lock;
    ObjectAction unlock;

    const LuaWorldObject* luaObject;
};

extern Object* objs;
extern Object* endOfObjs;

// TODO: this is for temporary compability, remove later
extern Object* heaven;
extern Object* field;
extern Object* cave;
extern Object* guard;
extern Object* player;

#define validObject(obj)     ((obj) != NULL && (obj)->condition(obj))

bool objectInitFromLuaWorld(void);
void objectFree(void);
Object* objectById(const char* id);
const char* objectGetLastError(void);

#endif
