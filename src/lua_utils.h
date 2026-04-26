#ifndef LUA_UTILS_H
#define LUA_UTILS_H

#include <lua.h>
#include <stdbool.h>

bool luaGetStringField(lua_State* L, int index, const char* key, char** out);
bool luaGetIntField(lua_State* L, int index, const char* key, int* out);
bool luaGetStringOrFunctionField(lua_State* L, int index, const char* key, char** nameOut, int* refOut);

char* dupString(const char* src);

#endif