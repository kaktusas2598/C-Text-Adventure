#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include <lua.h>

void luaRegisterEngineBindings(lua_State* lua, int worldRef);

#endif
