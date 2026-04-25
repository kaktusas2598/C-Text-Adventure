#ifndef LUA_WORLD_H
#define LUA_WORLD_H

#include <stdbool.h>
#include <stddef.h>

// TODO: possibly better to move World struct to object.h?
 typedef struct Object Object;

typedef struct {
    Object* objects;
    size_t count;
} LuaWorld; //TODO: Probably should rename to just World and use instead of objs and endOfObjs or integrate them here?

bool luaWorldLoad(const char* filename);
void luaWorldUnload(void);
const LuaWorld* luaWorldGet(void);
bool luaWorldEvaluateCondition(const Object* object, bool* result);

#endif
