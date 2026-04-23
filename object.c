#include "object.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "lua_world.h"
#include "toggle.h"


// TODO: Dont want to reference game objects in any way apart from maybe player in engine code. This is just a temporary hack to get something working and will be replaced with a more flexible system of object management that doesn't rely on hardcoded object slots or global variables.
static const char* objectOrder[] = {
    "heaven",
    "respawn",
    "heavenEWNS",
    "field",
    "cave",
    "silver",
    "gold",
    "guard",
    "player",
    "intoCave",
    "intoCaveBlocked",
    "exitCave",
    "wallField",
    "wallCave",
    "backroom",
    "wallBackroom",
    "openDoorToBackroom",
    "closedDoorToBackroom",
    "openDoorToCave",
    "closedDoorToCave",
    "openBox",
    "closedBox",
    "lockedBox",
    "keyForBox",
    "lampOff",
    "lampOn",
    "club",
    "dagger"
};

static const char* defaultContents = "You see";
static const char* defaultDetails = "You see nothing special.";
static const char* defaultTextGo = "You can't get much closer than this.";
static const char* defaultGossip = "I know nothing about that.";
static const int defaultWeight = 99;

Object objs[sizeof objectOrder / sizeof objectOrder[0]];
static char lastError[512];

static void setError(const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vsnprintf(lastError, sizeof lastError, fmt, args);
    va_end(args);
}

static bool alwaysTrue(Object* object) {
    (void)object;
    return true;
}

static bool luaCondition(Object* object) {
    bool result = true;

    if (object == NULL || object->luaObject == NULL) {
        setError("cannot evaluate condition for a null runtime object");
        return false;
    }

    if (!luaWorldEvaluateCondition(object->luaObject, &result)) {
        setError("Lua condition failed for '%s': %s", object->id, luaWorldGetLastError());
        fprintf(stderr, "%s\n", lastError);
        return false;
    }

    return result;
}

static ObjectAction resolveAction(const char* name, ObjectAction fallback) {
    if (name == NULL || *name == '\0') {
        return fallback;
    }

    if (strcmp(name, "cannotBeOpened") == 0) return cannotBeOpened;
    if (strcmp(name, "cannotBeClosed") == 0) return cannotBeClosed;
    if (strcmp(name, "cannotBeLocked") == 0) return cannotBeLocked;
    if (strcmp(name, "cannotBeUnlocked") == 0) return cannotBeUnlocked;
    if (strcmp(name, "isAlreadyOpen") == 0) return isAlreadyOpen;
    if (strcmp(name, "isAlreadyClosed") == 0) return isAlreadyClosed;
    if (strcmp(name, "isAlreadyLocked") == 0) return isAlreadyLocked;
    if (strcmp(name, "isAlreadyUnlocked") == 0) return isAlreadyUnlocked;
    if (strcmp(name, "isStillOpen") == 0) return isStillOpen;
    if (strcmp(name, "isStillLocked") == 0) return isStillLocked;
    if (strcmp(name, "toggleDoorToBackroom") == 0) return toggleDoorToBackroom;
    if (strcmp(name, "toggleDoorToCave") == 0) return toggleDoorToCave;
    if (strcmp(name, "toggleBox") == 0) return toggleBox;
    if (strcmp(name, "toggleBoxLock") == 0) return toggleBoxLock;
    if (strcmp(name, "toggleLamp") == 0) return toggleLamp;

    setError("unknown object action '%s'", name);
    fprintf(stderr, "%s.\n", lastError);
    return fallback;
}

Object* objectById(const char* id) {
    Object* object;

    if (id == NULL) {
        return NULL;
    }

    for (object = objs; object < endOfObjs; object++) {
        if (object->id != NULL && strcmp(object->id, id) == 0) {
            return object;
        }
    }

    return NULL;
}

static bool objectIdIsExpected(const char* id) {
    size_t i;

    for (i = 0; i < sizeof objectOrder / sizeof objectOrder[0]; i++) {
        if (strcmp(objectOrder[i], id) == 0) {
            return true;
        }
    }

    return false;
}

static bool validateWorldCoverage(void) {
    const LuaWorld* luaWorld = luaWorldGet();
    size_t i;

    if (luaWorld->count != sizeof objectOrder / sizeof objectOrder[0]) {
        setError(
            "world.lua defines %zu objects but the engine currently expects %zu named slots",
            luaWorld->count,
            sizeof objectOrder / sizeof objectOrder[0]
        );
        return false;
    }

    for (i = 0; i < luaWorld->count; i++) {
        if (!objectIdIsExpected(luaWorld->objects[i].id)) {
            setError("world.lua defines unsupported object '%s'", luaWorld->objects[i].id);
            return false;
        }
    }

    return true;
}

static bool populateObject(Object* object, const char* id) {
    const LuaWorldObject* source = luaWorldFind(id);

    if (source == NULL) {
        setError("missing object '%s' in world.lua", id);
        return false;
    }

    memset(object, 0, sizeof *object);
    object->id = source->id;
    object->condition = source->conditionRef >= 0 ? luaCondition : alwaysTrue;
    object->description = source->description;
    object->tags = (const char**)source->tags;
    object->details = source->details != NULL ? source->details : defaultDetails;
    object->contents = source->contents != NULL ? source->contents : defaultContents;
    object->textGo = source->textGo != NULL ? source->textGo : defaultTextGo;
    object->gossip = source->gossip != NULL ? source->gossip : defaultGossip;
    object->weight = source->weight != 0 ? source->weight : defaultWeight;
    object->capacity = source->capacity;
    object->health = source->health;
    object->light = source->light;
    object->impact = source->impact;
    object->trust = source->trust;
    object->open = resolveAction(source->onOpen, cannotBeOpened);
    object->close = resolveAction(source->onClose, cannotBeClosed);
    object->lock = resolveAction(source->onLock, cannotBeLocked);
    object->unlock = resolveAction(source->onUnlock, cannotBeUnlocked);
    object->luaObject = source;
    return true;
}

static bool resolveReference(Object** dest, const char* objectId, const char* fieldName, const char* targetId) {
    if (targetId == NULL) {
        *dest = NULL;
        return true;
    }

    *dest = objectById(targetId);
    if (*dest == NULL) {
        setError(
            "object '%s' refers to unknown %s '%s'",
            objectId,
            fieldName,
            targetId
        );
        return false;
    }

    return true;
}

static bool resolveReferences(void) {
    Object* object;

    for (object = objs; object < endOfObjs; object++) {
        const LuaWorldObject* source = object->luaObject;

        if (!resolveReference(&object->location, object->id, "location", source->location) ||
            !resolveReference(&object->destination, object->id, "destination", source->destination) ||
            !resolveReference(
                &object->prospect,
                object->id,
                "prospect",
                source->prospect != NULL ? source->prospect : source->destination
            )) {
            return false;
        }
    }

    return true;
}

bool objectInitFromLuaWorld(void) {
    size_t i;

    objectFree();
    lastError[0] = '\0';

    if (!validateWorldCoverage()) {
        return false;
    }

    for (i = 0; i < sizeof objectOrder / sizeof objectOrder[0]; i++) {
        if (!populateObject(&objs[i], objectOrder[i])) {
            objectFree();
            return false;
        }
    }

    if (!resolveReferences()) {
        objectFree();
        return false;
    }

    return true;
}

void objectFree(void) {
    memset(objs, 0, sizeof objs);
}

const char* objectGetLastError(void) {
    return lastError;
}
