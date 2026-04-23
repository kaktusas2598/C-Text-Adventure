#include "lua_bindings.h"

#include <stdbool.h>

#include <lauxlib.h>

#include <string.h>

#include "object.h"

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

static int l_engineGetObject(lua_State* lua) {
    int worldRef = (int)lua_tointeger(lua, lua_upvalueindex(1));
    const char* id = luaL_checkstring(lua, 1);
    pushWorldObjectById(lua, worldRef, id);
    return 1;
}

static int l_engineGetField(lua_State* lua) {
    int worldRef = (int)lua_tointeger(lua, lua_upvalueindex(1));
    const char* id = luaL_checkstring(lua, 1);
    const char* attributeName = luaL_checkstring(lua, 2);

    Object* object = objectById(id);
    if (object == NULL) {
        lua_pushnil(lua);
        return 1;
    }

    // TODO: add all fields here from Object?
    if (strcmp(attributeName, "location") == 0) {
        lua_pushstring(lua, object->location != NULL ? object->location->id : NULL);
        return 1;
    }
    if (strcmp(attributeName, "destination") == 0) {
        lua_pushstring(lua, object->destination != NULL ? object->destination->id : NULL);
        return 1;
    }
    if (strcmp(attributeName, "prospect") == 0) {
        lua_pushstring(lua, object->prospect != NULL ? object->prospect->id : NULL);
        return 1;
    }
    if (strcmp(attributeName, "health") == 0) {
        lua_pushinteger(lua, object->health);
        return 1;
    }
    if (strcmp(attributeName, "light") == 0) {
        lua_pushinteger(lua, object->light);
        return 1;
    }
    if (strcmp(attributeName, "trust") == 0) {
        lua_pushinteger(lua, object->trust);
        return 1;
    }
    if (strcmp(attributeName, "capacity") == 0) {
        lua_pushinteger(lua, object->capacity);
        return 1;
    }
    if (strcmp(attributeName, "weight") == 0) {
        lua_pushinteger(lua, object->weight);
        return 1;
    }

    if (!pushWorldObjectById(lua, worldRef, id)) {
        return 1;
    }

    lua_getfield(lua, -1, attributeName);
    lua_remove(lua, -2);
    return 1;
}

static int l_engineObjectExists(lua_State* lua) {
    int worldRef = (int)lua_tointeger(lua, lua_upvalueindex(1));
    const char* id = luaL_checkstring(lua, 1);
    bool found = pushWorldObjectById(lua, worldRef, id);
    lua_pop(lua, 1);
    lua_pushboolean(lua, found);
    return 1;
}

void registerEngineBindings(lua_State* lua, int worldRef) {
    lua_newtable(lua);

    lua_pushinteger(lua, worldRef);
    lua_pushcclosure(lua, l_engineGetObject, 1);
    lua_setfield(lua, -2, "get_object");

    lua_pushinteger(lua, worldRef);
    lua_pushcclosure(lua, l_engineGetField, 1);
    lua_setfield(lua, -2, "get_field");

    lua_pushinteger(lua, worldRef);
    lua_pushcclosure(lua, l_engineObjectExists, 1);
    lua_setfield(lua, -2, "object_exists");

    lua_setglobal(lua, "engine");
}
