#include "lua_world.h"
#include "lua_bindings.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static LuaWorld world;
static char lastError[512];
static lua_State* worldLua;
static int worldRef = LUA_NOREF;

static void setError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(lastError, sizeof lastError, fmt, args);
    va_end(args);
}

// Used for copying Lua strings because runtime object data outlives stack usage
static char* dupString(const char* src) {
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

static void freeObject(LuaWorldObject* object) {
    size_t i;

    free(object->id);
    free(object->kind);
    free(object->description);
    free(object->location);
    free(object->destination);
    free(object->prospect);
    free(object->deathDestination);
    free(object->dropDestination);
    free(object->togglesTo);
    free(object->mirrorsTo);
    free(object->locksTo);
    free(object->keyId);
    free(object->details);
    free(object->contents);
    free(object->textGo);
    free(object->gossip);
    free(object->condition);
    free(object->onOpen);
    free(object->onClose);
    free(object->onLock);
    free(object->onUnlock);

    for (i = 0; i < object->tagCount; i++) {
        free(object->tags[i]);
    }
    free(object->tags);

    memset(object, 0, sizeof *object);
}

static void resetLuaState(void) {
    if (worldLua != NULL) {
        lua_close(worldLua);
        worldLua = NULL;
    }
    worldRef = LUA_NOREF;
}

void luaWorldUnload(void) {
    size_t i;

    for (i = 0; i < world.count; i++) {
        freeObject(&world.objects[i]);
    }
    free(world.objects);
    memset(&world, 0, sizeof world);
    resetLuaState();
    lastError[0] = '\0';
}

const LuaWorld* luaWorldGet(void) {
    return &world;
}

const char* luaWorldGetLastError(void) {
    return lastError;
}

const LuaWorldObject* luaWorldFind(const char* id) {
    size_t i;

    if (id == NULL) {
        return NULL;
    }

    for (i = 0; i < world.count; i++) {
        if (world.objects[i].id != NULL && strcmp(world.objects[i].id, id) == 0) {
            return &world.objects[i];
        }
    }

    return NULL;
}

static bool fileExists(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}

static bool setStringField(lua_State* lua, int index, const char* key, char** dest) {
    const char* value;

    lua_getfield(lua, index, key);
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        return true;
    }
    if (!lua_isstring(lua, -1)) {
        setError("objects[] field '%s' must be a string", key);
        lua_pop(lua, 1);
        return false;
    }

    value = lua_tostring(lua, -1);
    *dest = dupString(value);
    lua_pop(lua, 1);

    if (*dest == NULL) {
        setError("out of memory while reading field '%s'", key);
        return false;
    }
    return true;
}

// Either a named string or lua runction reference
static bool setBehaviorField(lua_State* lua, int index, const char* key, char** nameDest, int* refDest) {
    const char* value;

    lua_getfield(lua, index, key);
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        return true;
    }

    if (lua_isstring(lua, -1)) {
        value = lua_tostring(lua, -1);
        *nameDest = dupString(value);
        lua_pop(lua, 1);
        if (*nameDest == NULL) {
            setError("out of memory while reading field '%s'", key);
            return false;
        }
        return true;
    }

    if (lua_isfunction(lua, -1)) {
        *refDest = luaL_ref(lua, LUA_REGISTRYINDEX);
        return true;
    }

    setError("objects[] field '%s' must be a string or function", key);
    lua_pop(lua, 1);
    return false;
}

static bool setIntField(lua_State* lua, int index, const char* key, int* dest) {
    lua_getfield(lua, index, key);
    if (lua_isnil(lua, -1)) {
        lua_pop(lua, 1);
        return true;
    }
    if (!lua_isinteger(lua, -1)) {
        setError("objects[] field '%s' must be an integer", key);
        lua_pop(lua, 1);
        return false;
    }
    *dest = (int)lua_tointeger(lua, -1);
    lua_pop(lua, 1);
    return true;
}

static bool loadTags(lua_State* lua, int index, LuaWorldObject* object) {
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

static bool validateObject(const LuaWorldObject* object, size_t index) {
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

    // ???
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

// Objet can be loaded either using explicit "id" field or from the table key
static bool loadObject(lua_State* lua, int index, const char* fallbackId, LuaWorldObject* object, size_t objectIndex) {
    object->conditionRef = LUA_NOREF;
    object->onOpenRef = LUA_NOREF;
    object->onCloseRef = LUA_NOREF;
    object->onLockRef = LUA_NOREF;
    object->onUnlockRef = LUA_NOREF;

    if (!setStringField(lua, index, "id", &object->id)) {
        return false;
    }
    if (object->id == NULL && fallbackId != NULL) {
        object->id = dupString(fallbackId);
        if (object->id == NULL) {
            setError("out of memory while reading object id");
            return false;
        }
    }

    if (!setStringField(lua, index, "kind", &object->kind) ||
        !setStringField(lua, index, "description", &object->description) ||
        !loadTags(lua, index, object) ||
        !setStringField(lua, index, "location", &object->location) ||
        !setStringField(lua, index, "destination", &object->destination) ||
        !setStringField(lua, index, "prospect", &object->prospect) ||
        !setStringField(lua, index, "death_destination", &object->deathDestination) ||
        !setStringField(lua, index, "drop_destination", &object->dropDestination) ||
        !setStringField(lua, index, "toggles_to", &object->togglesTo) ||
        !setStringField(lua, index, "mirrors_to", &object->mirrorsTo) ||
        !setStringField(lua, index, "locks_to", &object->locksTo) ||
        !setStringField(lua, index, "key_id", &object->keyId) ||
        !setStringField(lua, index, "details", &object->details) ||
        !setStringField(lua, index, "contents", &object->contents) ||
        !setStringField(lua, index, "text_go", &object->textGo) ||
        !setStringField(lua, index, "gossip", &object->gossip) ||
        !setBehaviorField(lua, index, "condition", &object->condition, &object->conditionRef) ||
        !setBehaviorField(lua, index, "on_open", &object->onOpen, &object->onOpenRef) ||
        !setBehaviorField(lua, index, "on_close", &object->onClose, &object->onCloseRef) ||
        !setBehaviorField(lua, index, "on_lock", &object->onLock, &object->onLockRef) ||
        !setBehaviorField(lua, index, "on_unlock", &object->onUnlock, &object->onUnlockRef) ||
        !setIntField(lua, index, "weight", &object->weight) ||
        !setIntField(lua, index, "capacity", &object->capacity) ||
        !setIntField(lua, index, "health", &object->health) ||
        !setIntField(lua, index, "light", &object->light) ||
        !setIntField(lua, index, "impact", &object->impact) ||
        !setIntField(lua, index, "trust", &object->trust)) {
        return false;
    }

    return validateObject(object, objectIndex);
}

bool luaWorldLoad(const char* filename) {
    lua_State* lua;
    size_t count;
    size_t i;
    bool ok = false;

    luaWorldUnload();
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

    if (!lua_istable(lua, -1)) {
        setError("world.lua must return a table");
        resetLuaState();
        return false;
    }

    lua_pushvalue(lua, -1);
    worldRef = luaL_ref(lua, LUA_REGISTRYINDEX);
    registerEngineBindings(lua, worldRef);

    lua_getfield(lua, -1, "objects");
    if (!lua_istable(lua, -1)) {
        setError("world.lua must define an 'objects' table");
        lua_pop(lua, 2);
        resetLuaState();
        return false;
    }

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
            // TODO: Need to get rid of this horriblness
            goto cleanup;
        }

        objectId = lua_tostring(lua, -2);
        if (!loadObject(lua, -1, objectId, &world.objects[i], i)) {
            lua_pop(lua, 3);
            goto cleanup;
        }
        i++;
        lua_pop(lua, 1);
    }

    if (!validateUniqueIds()) {
        lua_pop(lua, 2);
        goto cleanup;
    }

    lua_pop(lua, 2);
    ok = true;

cleanup:
    if (!ok) {
        luaWorldUnload();
    }
    return ok;
}

bool luaWorldEvaluateCondition(const LuaWorldObject* object, bool* result) {
    int status;

    if (result != NULL) {
        *result = true;
    }

    if (object == NULL) {
        setError("cannot evaluate condition for a null object");
        return false;
    }

    if (object->conditionRef == LUA_NOREF) {
        return true;
    }

    if (worldLua == NULL || worldRef == LUA_NOREF) {
        setError("Lua world is not loaded");
        return false;
    }

    lua_rawgeti(worldLua, LUA_REGISTRYINDEX, object->conditionRef);
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
