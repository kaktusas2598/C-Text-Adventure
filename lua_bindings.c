#include "lua_bindings.h"

#include <stdbool.h>

#include <lauxlib.h>

static bool pushWorldObjectById(lua_State* lua, int worldRef, const char* id) {
    if (worldRef == LUA_NOREF) {
        lua_pushnil(lua);
        return false;
    }

    lua_rawgeti(lua, LUA_REGISTRYINDEX, worldRef);
    lua_getfield(lua, -1, "objects");
    lua_remove(lua, -2);

    if (!lua_istable(lua, -1)) {
        lua_pop(lua, 1);
        lua_pushnil(lua);
        return false;
    }

    lua_getfield(lua, -1, id);
    lua_remove(lua, -2);
    return !lua_isnil(lua, -1);
}

static int luaEngineGetObject(lua_State* lua) {
    int worldRef = (int)lua_tointeger(lua, lua_upvalueindex(1));
    const char* id = luaL_checkstring(lua, 1);
    pushWorldObjectById(lua, worldRef, id);
    return 1;
}

static int luaEngineGetField(lua_State* lua) {
    int worldRef = (int)lua_tointeger(lua, lua_upvalueindex(1));
    const char* id = luaL_checkstring(lua, 1);
    const char* field = luaL_checkstring(lua, 2);

    if (!pushWorldObjectById(lua, worldRef, id)) {
        return 1;
    }

    lua_getfield(lua, -1, field);
    lua_remove(lua, -2);
    return 1;
}

static int luaEngineObjectExists(lua_State* lua) {
    int worldRef = (int)lua_tointeger(lua, lua_upvalueindex(1));
    const char* id = luaL_checkstring(lua, 1);
    bool found = pushWorldObjectById(lua, worldRef, id);
    lua_pop(lua, 1);
    lua_pushboolean(lua, found);
    return 1;
}

void luaRegisterEngineBindings(lua_State* lua, int worldRef) {
    lua_newtable(lua);

    lua_pushinteger(lua, worldRef);
    lua_pushcclosure(lua, luaEngineGetObject, 1);
    lua_setfield(lua, -2, "get_object");

    lua_pushinteger(lua, worldRef);
    lua_pushcclosure(lua, luaEngineGetField, 1);
    lua_setfield(lua, -2, "get_field");

    lua_pushinteger(lua, worldRef);
    lua_pushcclosure(lua, luaEngineObjectExists, 1);
    lua_setfield(lua, -2, "object_exists");

    lua_setglobal(lua, "engine");
}
