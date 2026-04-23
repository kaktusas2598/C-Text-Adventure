#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include <lua.h>

void registerEngineBindings(lua_State* lua, int worldRef);

#endif
