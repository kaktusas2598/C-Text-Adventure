#include "lua_utils.h"

#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "error.h"

// Used for copying Lua strings because runtime object data outlives stack usage
char* dupString(const char* src) {
    size_t size;
    char* copy;

    if (src == NULL) {
        return NULL;
    }

    size = strlen(src) + 1;
    copy = malloc(size);
    if (copy != NULL) {
        memcpy(copy, src, size);
    }
    return copy;
}

bool luaGetStringField(lua_State* L, int index, const char* key, char** out) {
// static bool setStringField(lua_State* lua, int index, const char* key, char** dest) {
    const char* value;

    lua_getfield(L, index, key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return true;
    }
    if (!lua_isstring(L, -1)) {
        setError("objects[] field '%s' must be a string", key);
        lua_pop(L, 1);
        return false;
    }

    value = lua_tostring(L, -1);
    *out = dupString(value);
    lua_pop(L, 1);

    if (*out == NULL) {
        setError("out of memory while reading field '%s'", key);
        return false;
    }
    return true;
}

bool luaGetIntField(lua_State* L, int index, const char* key, int* out) {
    lua_getfield(L, index, key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return true;
    }
    if (!lua_isinteger(L, -1)) {
        setError("objects[] field '%s' must be an integer", key);
        lua_pop(L, 1);
        return false;
    }
    *out = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
    return true;
}

// Either a named string or lua runction reference
bool luaGetStringOrFunctionField(lua_State* L, int index, const char* key, char** nameOut, int* refOut) {
    const char* value;

    lua_getfield(L, index, key);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return true;
    }

    if (lua_isstring(L, -1)) {
        value = lua_tostring(L, -1);
        *nameOut = dupString(value);
        lua_pop(L, 1);
        if (*nameOut == NULL) {
            setError("out of memory while reading field '%s'", key);
            return false;
        }
        return true;
    }

    if (lua_isfunction(L, -1)) {
        *refOut = luaL_ref(L, LUA_REGISTRYINDEX);
        return true;
    }

    setError("objects[] field '%s' must be a string or function", key);
    lua_pop(L, 1);
    return false;
}
