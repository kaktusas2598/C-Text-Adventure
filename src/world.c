#include "world.h"
#include "lua_bindings.h"
#include "file_utils.h"
#include "error.h"
#include "lua_utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Temporary?
#include "object.h"

static World world;
static lua_State* worldLua; // Global Lua state
static int worldRef = LUA_NOREF;


static void resetLuaState(void) {
    if (worldLua != NULL) {
        lua_close(worldLua);
        worldLua = NULL;
    }
    worldRef = LUA_NOREF;
}

void worldUnload(void) {
    free(world.objects);
    memset(&world, 0, sizeof world);
    resetLuaState();

    world.objects = NULL;
}

const World* worldGet(void) {
    return &world;
}

static bool loadTags(lua_State* lua, int index, Object* object) {
    size_t count;
    size_t i;

    lua_getfield(lua, index, "tags");
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        return true;
    }
    if (!lua_istable(lua, -1)) {
        setError("objects[] field 'tags' must be a table");
        lua_pop(lua, 1);
        return false;
    }

    count = (size_t)lua_rawlen(lua, -1);
    if (count == 0) {
        lua_pop(lua, 1);
        return true;
    }

    object->tags = calloc(count + 1, sizeof *object->tags);
    if (object->tags == NULL) {
        setError("out of memory while reading tags");
        lua_pop(lua, 1);
        return false;
    }

    for (i = 0; i < count; i++) {
        lua_rawgeti(lua, -1, (lua_Integer)i + 1);
        if (!lua_isstring(lua, -1)) {
            setError("tags[%zu] must be a string", i + 1);
            lua_pop(lua, 2);
            return false;
        }

        object->tags[i] = dupString(lua_tostring(lua, -1));
        lua_pop(lua, 1);
        if (object->tags[i] == NULL) {
            setError("out of memory while reading tags");
            lua_pop(lua, 1);
            return false;
        }
        object->tagCount++;
    }

    lua_pop(lua, 1);
    return true;
}

static bool validateObject(const Object* object, size_t index) {
    if (object->id == NULL || *object->id == '\0') {
        setError("objects[%zu] is missing a non-empty 'id'", index + 1);
        return false;
    }
    if (object->kind == NULL || *object->kind == '\0') {
        setError("objects[%zu] is missing a non-empty 'kind'", index + 1);
        return false;
    }
    if (object->description == NULL || *object->description == '\0') {
        setError("objects[%zu] is missing a non-empty 'description'", index + 1);
        return false;
    }
    return true;
}

static size_t countNamedObjects(lua_State* lua, int index) {
    size_t count = 0;

    lua_pushnil(lua);
    while (lua_next(lua, index) != 0) {
        if (lua_type(lua, -2) == LUA_TSTRING && lua_istable(lua, -1)) {
            count++;
        }
        lua_pop(lua, 1);
    }

    return count;
}

static bool validateUniqueIds(void) {
    size_t i;
    size_t j;

    for (i = 0; i < world.count; i++) {
        for (j = i + 1; j < world.count; j++) {
            if (strcmp(world.objects[i].id, world.objects[j].id) == 0) {
                setError("duplicate object id '%s' in world.lua", world.objects[i].id);
                return false;
            }
        }
    }

    return true;
}

// Object can be loaded either using explicit "id" field or from the table key
static bool loadObject(lua_State* lua, int index, const char* fallbackId, Object* object, size_t objectIndex) {
    object->luaMetadata.conditionRef = LUA_NOREF;
    object->luaMetadata.onOpenRef = LUA_NOREF;
    object->luaMetadata.onCloseRef = LUA_NOREF;
    object->luaMetadata.onLockRef = LUA_NOREF;
    object->luaMetadata.onUnlockRef = LUA_NOREF;

    if (!luaGetStringField(lua, index, "id", &object->id)) {
        return false;
    }
    if (object->id == NULL && fallbackId != NULL) {
        object->id = dupString(fallbackId);
        if (object->id == NULL) {
            setError("out of memory while reading object id");
            return false;
        }
    }

    if (!luaGetStringField(lua, index, "kind", &object->kind) ||
        !luaGetStringField(lua, index, "description", &object->description) ||
        !loadTags(lua, index, object) ||
        !luaGetStringField(lua, index, "location", &object->luaMetadata.locationId) ||
        !luaGetStringField(lua, index, "destination", &object->luaMetadata.destinationId) ||
        !luaGetStringField(lua, index, "prospect", &object->luaMetadata.prospectId) ||
        !luaGetStringField(lua, index, "death_destination", &object->luaMetadata.deathDestinationId) ||
        !luaGetStringField(lua, index, "drop_destination", &object->luaMetadata.dropDestinationId) ||
        !luaGetStringField(lua, index, "toggles_to", &object->luaMetadata.togglesToId) ||
        !luaGetStringField(lua, index, "mirrors_to", &object->luaMetadata.mirrorsToId) ||
        !luaGetStringField(lua, index, "locks_to", &object->luaMetadata.locksToId) ||
        !luaGetStringField(lua, index, "key_id", &object->luaMetadata.keyId) ||
        !luaGetStringField(lua, index, "details", &object->details) ||
        !luaGetStringField(lua, index, "contents", &object->contents) ||
        !luaGetStringField(lua, index, "text_go", &object->textGo) ||
        !luaGetStringField(lua, index, "gossip", &object->gossip) ||
        !luaGetStringOrFunctionField(lua, index, "condition", &object->luaMetadata.condition, &object->luaMetadata.conditionRef) ||
        !luaGetStringOrFunctionField(lua, index, "on_open", &object->luaMetadata.onOpen, &object->luaMetadata.onOpenRef) ||
        !luaGetStringOrFunctionField(lua, index, "on_close", &object->luaMetadata.onClose, &object->luaMetadata.onCloseRef) ||
        !luaGetStringOrFunctionField(lua, index, "on_lock", &object->luaMetadata.onLock, &object->luaMetadata.onLockRef) ||
        !luaGetStringOrFunctionField(lua, index, "on_unlock", &object->luaMetadata.onUnlock, &object->luaMetadata.onUnlockRef) ||
        !luaGetIntField(lua, index, "weight", &object->weight) ||
        !luaGetIntField(lua, index, "capacity", &object->capacity) ||
        !luaGetIntField(lua, index, "health", &object->health) ||
        !luaGetIntField(lua, index, "light", &object->light) ||
        !luaGetIntField(lua, index, "impact", &object->impact) ||
        !luaGetIntField(lua, index, "trust", &object->trust)) {
        return false;
    }

    return validateObject(object, objectIndex);
}

bool worldLoad(const char* filename) {
    lua_State* lua;
    size_t count;
    size_t i;

    worldUnload();
    if (filename == NULL || !fileExists(filename)) {
        return true;
    }

    worldLua = luaL_newstate();
    if (worldLua == NULL) {
        setError("failed to create Lua state");
        return false;
    }
    lua = worldLua;

    luaL_openlibs(lua);

    if (luaL_dofile(lua, filename) != LUA_OK) {
        setError("%s", lua_tostring(lua, -1));
        resetLuaState();
        return false;
    }

    // Check if main user provided script returns a table
    if (!lua_istable(lua, -1)) {
        setError("%s must return a table", filename);
        resetLuaState();
        return false;
    }

    lua_pushvalue(lua, -1);
    worldRef = luaL_ref(lua, LUA_REGISTRYINDEX);
    registerEngineBindings(lua, worldRef);

    // Check if 'objects' field is set and defines a table in main table returned
    lua_getfield(lua, -1, "objects");
    if (!lua_istable(lua, -1)) {
        setError("%s must define an 'objects' table", filename);
        lua_pop(lua, 2);
        resetLuaState();
        return false;
    }

    // Count all objects in table 'objects' and dynamically allocate memory for world
    count = countNamedObjects(lua, lua_gettop(lua));
    world.objects = calloc(count, sizeof *world.objects);
    if (count > 0 && world.objects == NULL) {
        setError("out of memory while loading world objects");
        lua_pop(lua, 2);
        resetLuaState();
        return false;
    }

    world.count = count;
    i = 0;
    // Load all Object pointers in the World
    lua_pushnil(lua);
    while (lua_next(lua, -2) != 0) {
        const char* objectId;

        if (lua_type(lua, -2) != LUA_TSTRING) {
            lua_pop(lua, 1);
            continue;
        }

        if (!lua_istable(lua, -1)) {
            setError("objects['%s'] must be a table", lua_tostring(lua, -2));
            lua_pop(lua, 3);
            worldUnload();
            return false;
        }

        objectId = lua_tostring(lua, -2);
        if (!loadObject(lua, -1, objectId, &world.objects[i], i)) {
            lua_pop(lua, 3);
            worldUnload();
            return false;
        }
        i++;
        lua_pop(lua, 1);
    }

    if (!validateUniqueIds()) {
        lua_pop(lua, 2);
        worldUnload();
        return false;
    }

    lua_pop(lua, 2);
    return true;
}

bool worldEvaluateCondition(const Object* object, bool* result) {
    int status;

    if (result != NULL) {
        *result = true;
    }

    if (object == NULL) {
        setError("cannot evaluate condition for a null object");
        return false;
    }

    if (object->luaMetadata.conditionRef == LUA_NOREF) {
        return true;
    }

    if (worldLua == NULL || worldRef == LUA_NOREF) {
        setError("Lua world is not loaded");
        return false;
    }

    lua_rawgeti(worldLua, LUA_REGISTRYINDEX, object->luaMetadata.conditionRef);
    lua_rawgeti(worldLua, LUA_REGISTRYINDEX, worldRef);
    lua_pushstring(worldLua, object->id);

    status = lua_pcall(worldLua, 2, 1, 0);
    if (status != LUA_OK) {
        setError("error while evaluating condition for '%s': %s", object->id, lua_tostring(worldLua, -1));
        lua_pop(worldLua, 1);
        return false;
    }

    if (!lua_isboolean(worldLua, -1)) {
        setError("condition for '%s' must return true or false", object->id);
        lua_pop(worldLua, 1);
        return false;
    }

    if (result != NULL) {
        *result = lua_toboolean(worldLua, -1) != 0;
    }
    lua_pop(worldLua, 1);
    return true;
}

bool worldLoadObjects(void) {
    const World* luaWorld;
    size_t i;

    luaWorld = worldGet();
    if (luaWorld == NULL) {
        setError("Lua world is not loaded");
        return false;
    }

    // TEMP
    objs = luaWorld->objects;
    endOfObjs = objs + luaWorld->count;

    for(Object* o = luaWorld->objects; o < luaWorld->objects + luaWorld->count; o++)
        initializeRuntimeFields(o);

    // Resolve Object pointer references (location, destination, prospect)
    for(Object* o = luaWorld->objects; o < luaWorld->objects + luaWorld->count; o++)
        if (!resolveReferences(o))
            return false;
    

    // Resolve required objects, for now only player is required to be defined
    player = objectById("player");
    if (player == NULL) {
        setError("object 'player' is required to be defined in Lua game file.");
        return false;
    }

    return true;
}
