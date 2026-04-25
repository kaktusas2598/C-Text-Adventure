#ifndef LUA_WORLD_H
#define LUA_WORLD_H

#include <stdbool.h>
#include <stddef.h>

// TODO: possibly better to move World struct to object.h?
 typedef struct Object Object;

typedef struct {
    Object* objects;
    size_t count;
} World;

bool luaWorldLoad(const char* filename);
void luaWorldUnload(void);
const World* luaWorldGet(void);
bool luaWorldEvaluateCondition(const Object* object, bool* result);

#endif
