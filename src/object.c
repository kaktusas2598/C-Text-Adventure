#include "object.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua_world.h"
#include "toggle.h"
#include "colour.h"
#include "error.h"

static const char* defaultContents = "You see";
static const char* defaultDetails = "You see nothing special.";
static const char* defaultTextGo = "You can't get much closer than this.";
static const char* defaultGossip = "I know nothing about that.";
static const int defaultWeight = 99;

Object* objs = NULL;
Object* endOfObjs = NULL;
Object* player = NULL;

static bool alwaysTrue(Object* object) {
    (void)object;
    return true;
}

static bool luaCondition(Object* object) {
    bool result = true;

    if (object == NULL) {
        setError("cannot evaluate condition for a null runtime object");
        return false;
    }

    if (!luaWorldEvaluateCondition(object, &result)) {
        setError("Lua condition failed for '%s': %s", object->id, getLastError());
        fprintf(stderr, "%s%s%s\n", getLastError(), RED, RESET);
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
    if (strcmp(name, "openViaToggle") == 0) return openViaToggle;
    if (strcmp(name, "closeViaToggle") == 0) return closeViaToggle;
    if (strcmp(name, "lockViaToggle") == 0) return lockViaToggle;
    if (strcmp(name, "unlockViaToggle") == 0) return unlockViaToggle;
    

    setError("unknown object action '%s'", name);
    fprintf(stderr, "%s.\n", getLastError());
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

static void initializeRuntimeFields(void) {
    Object* object;

    for (object = objs; object < endOfObjs; object++) {
        object->condition = object->luaMetadata.conditionRef >= 0 ? luaCondition : alwaysTrue;

        object->details = object->details != NULL ? object->details : (char*)defaultDetails;
        object->contents = object->contents != NULL ? object->contents : (char*)defaultContents;
        object->textGo = object->textGo != NULL ? object->textGo : (char*)defaultTextGo;
        object->gossip = object->gossip != NULL ? object->gossip : (char*)defaultGossip;
        object->weight = object->weight != 0 ? object->weight : defaultWeight;

        object->open = resolveAction(object->luaMetadata.onOpen, cannotBeOpened);
        object->close = resolveAction(object->luaMetadata.onClose, cannotBeClosed);
        object->lock = resolveAction(object->luaMetadata.onLock, cannotBeLocked);
        object->unlock = resolveAction(object->luaMetadata.onUnlock, cannotBeUnlocked);
    }
}

// Resolve string ids such as location/destination into Object pointers.
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
        if (!resolveReference(&object->location, object->id, "location", object->luaMetadata.locationId) ||
            !resolveReference(&object->destination, object->id, "destination", object->luaMetadata.destinationId) ||
            !resolveReference(&object->togglesTo, object->id, "toggles_to", object->luaMetadata.togglesToId) ||
            !resolveReference(&object->mirrorsTo, object->id, "mirrors_to", object->luaMetadata.mirrorsToId) ||
            !resolveReference(&object->locksTo, object->id, "locks_to", object->luaMetadata.locksToId) ||
            !resolveReference(&object->key, object->id, "key_id", object->luaMetadata.keyId) ||
            !resolveReference(&object->deathDestination, object->id, "death_destination", object->luaMetadata.deathDestinationId) ||
            !resolveReference(&object->dropDestination, object->id, "drop_destination", object->luaMetadata.dropDestinationId) ||
            !resolveReference(
                &object->prospect,
                object->id,
                "prospect",
                object->luaMetadata.prospectId != NULL ? object->luaMetadata.prospectId : object->luaMetadata.destinationId
            )) {
            return false;
        }
    }

    return true;
}

bool objectInitFromLuaWorld(void) {
    const World* luaWorld;
    size_t i;

    objectFree();
    // getLastError()[0] = '\0';

    luaWorld = luaWorldGet();
    if (luaWorld == NULL) {
        setError("Lua world is not loaded");
        return false;
    }

    // LuaWorld struct owns objects memory, just assigning it here to track it in the engine
    objs = luaWorld->objects;
    endOfObjs = objs + luaWorld->count;

    initializeRuntimeFields();

    // Resolve Object pointer references (location, destination, prospect)
    if (!resolveReferences()) {
        objectFree();
        return false;
    }

    // Resolve required objects, for now only player is required to be defined
    player = objectById("player");
    if (player == NULL) {
        objectFree();
        setError("object 'player' is required to be defined in Lua game file.");
        return false;
    }

    return true;
}

void objectFree(void) {
    objs = NULL;
    endOfObjs = NULL;
    player = NULL;
}