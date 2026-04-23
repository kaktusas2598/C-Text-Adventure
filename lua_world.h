#ifndef LUA_WORLD_H
#define LUA_WORLD_H

#include <stdbool.h>
#include <stddef.h>

typedef struct LuaWorldObject {
    char* id;
    char* kind;
    char* description;
    char** tags;
    size_t tagCount;
    char* location;
    char* destination;
    char* prospect;
    char* details;
    char* contents;
    char* textGo;
    char* gossip;
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
    int weight;
    int capacity;
    int health;
    int light;
    int impact;
    int trust;
} LuaWorldObject;

typedef struct {
    LuaWorldObject* objects;
    size_t count;
} LuaWorld;

bool luaWorldLoad(const char* filename);
void luaWorldUnload(void);
const LuaWorld* luaWorldGet(void);
const LuaWorldObject* luaWorldFind(const char* id);
bool luaWorldEvaluateCondition(const LuaWorldObject* object, bool* result);
const char* luaWorldGetLastError(void);

#endif
